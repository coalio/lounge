#include "engine/backend/telegram/telegram_backend.hpp"

#include <charconv>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <system_error>

#include <td/telegram/Client.h>

#include "engine/config/config.hpp"

namespace engine::backend::telegram {

namespace {

constexpr float kReceiveTimeoutSeconds = 1.0F;

}  // namespace

TelegramBackend::TelegramBackend(engine::events::EventService& events, std::string source)
    : Backend(events, std::move(source)) {}

TelegramBackend::~TelegramBackend() {
    stop();
}

auto TelegramBackend::start() -> std::expected<void, std::string> {
    // we silence tdlib logs but we'll probably have to enable this/handle them later
    td::ClientManager::execute(
        td::td_api::make_object<td::td_api::setLogStream>(
            td::td_api::make_object<td::td_api::logStreamEmpty>()
        )
    );
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(0));

    emit(engine::events::EventId::BackendStatus,
         BackendStatus{BackendStatusKind::Connecting, "Connecting to Telegram"});

    client_id_ = client_manager_.create_client_id();
    running_ = true;

    send_query(td::td_api::make_object<td::td_api::getAuthorizationState>());

    receiver_thread_ = std::thread(&TelegramBackend::receiver_loop, this);

    std::unique_lock lock(auth_mutex_);
    auth_cv_.wait(lock, [this] { return auth_ready_ || auth_failed_; });

    if (auth_failed_) {
        running_ = false;
        return std::unexpected(auth_error_);
    }

    return {};
}

void TelegramBackend::stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    send_query(td::td_api::make_object<td::td_api::close>());

    if (receiver_thread_.joinable()) {
        receiver_thread_.join();
    }

    emit(engine::events::EventId::BackendStatus,
         BackendStatus{BackendStatusKind::Stopped, "Backend stopped"});
}

void TelegramBackend::request_chats(std::int32_t limit) {
    if (!authorized_) {
        return;
    }

    send_query(td::td_api::make_object<td::td_api::getChats>(nullptr, limit));
}

void TelegramBackend::request_history(ChatId chat_id, std::int32_t limit) {
    if (!authorized_) {
        return;
    }

    const std::int32_t clamped_limit = limit <= 0 ? 10 : std::min<std::int32_t>(limit, 50);

    {
        std::lock_guard lock(history_mutex_);
        pending_history_[chat_id].clear();
        pending_history_limits_[chat_id] = clamped_limit;
    }

    send_query(td::td_api::make_object<td::td_api::getChatHistory>(
        chat_id,
        std::numeric_limits<std::int64_t>::max(),  // start from the latest known message
        0,                                      // no offset
        clamped_limit,
        false
    ));
}

void TelegramBackend::send_message(ChatId chat_id, std::string_view text) {
    if (!authorized_) {
        return;
    }

    auto send_message = td::td_api::make_object<td::td_api::sendMessage>();
    send_message->chat_id_ = chat_id;

    auto content = td::td_api::make_object<td::td_api::inputMessageText>();
    content->text_ = td::td_api::make_object<td::td_api::formattedText>();
    content->text_->text_ = std::string{text};
    send_message->input_message_content_ = std::move(content);

    send_query(std::move(send_message));
}

void TelegramBackend::receiver_loop() {
    while (running_) {
        auto response = client_manager_.receive(kReceiveTimeoutSeconds);
        if (!response.object) {
            continue;
        }

        process_response(std::move(response));
    }
}

void TelegramBackend::process_response(td::ClientManager::Response response) {
    if (response.request_id == 0) {
        handle_update(std::move(response.object));
        return;
    }

    handle_response(response.request_id, std::move(response.object));
}

void TelegramBackend::handle_update(TdObject update) {
    const auto id = update->get_id();

    if (id == td::td_api::updateAuthorizationState::ID) {
        auto state = td::td_api::move_object_as<td::td_api::updateAuthorizationState>(std::move(update));
        handle_authorization_state(std::move(state->authorization_state_));
        return;
    }

    if (id == td::td_api::updateNewMessage::ID) {
        auto new_message = td::td_api::move_object_as<td::td_api::updateNewMessage>(std::move(update));
        handle_new_message(std::move(new_message));
        return;
    }

    if (id == td::td_api::updateChatTitle::ID) {
        auto chat_title = td::td_api::move_object_as<td::td_api::updateChatTitle>(std::move(update));
        chat_titles_[chat_title->chat_id_] = chat_title->title_;
        return;
    }

    if (id == td::td_api::updateNewChat::ID) {
        auto new_chat = td::td_api::move_object_as<td::td_api::updateNewChat>(std::move(update));
        if (new_chat->chat_ != nullptr) {
            chat_titles_[new_chat->chat_->id_] = new_chat->chat_->title_;
        }
    }
}

void TelegramBackend::handle_response(std::int64_t request_id, TdObject object) {
    if (object->get_id() == td::td_api::error::ID) {
        auto error = td::td_api::move_object_as<td::td_api::error>(object);
        if (!auth_ready_) {
            std::lock_guard lock(auth_mutex_);
            auth_failed_ = true;
            auth_error_ = "TDLib error: " + error->message_;
            auth_cv_.notify_all();
        }
        emit(engine::events::EventId::BackendStatus,
             BackendStatus{BackendStatusKind::Error, error->message_});
        return;
    }

    const auto object_id = object->get_id();

    if (object_id == td::td_api::chats::ID) {
        handle_chats(td::td_api::move_object_as<td::td_api::chats>(object));
        return;
    }

    if (object_id == td::td_api::chat::ID) {
        handle_chat(td::td_api::move_object_as<td::td_api::chat>(object));
        return;
    }

    if (object_id == td::td_api::messages::ID) {
        handle_messages(td::td_api::move_object_as<td::td_api::messages>(object));
        return;
    }
}

void TelegramBackend::handle_authorization_state(td::td_api::object_ptr<td::td_api::AuthorizationState> state) {
    if (state == nullptr) {
        return;
    }

    const auto id = state->get_id();

    if (id == td::td_api::authorizationStateReady::ID) {
        authorized_ = true;
        {
            std::lock_guard lock(auth_mutex_);
            auth_ready_ = true;
        }
        auth_cv_.notify_all();

        emit(engine::events::EventId::BackendStatus,
             BackendStatus{BackendStatusKind::Ready, "Authorized"});
        
        return;
    }

    if (id == td::td_api::authorizationStateWaitTdlibParameters::ID) {
        send_tdlib_parameters();
        return;
    }

    if (id == td::td_api::authorizationStateWaitPhoneNumber::ID) {
        prompt_phone_number();
        return;
    }

    if (id == td::td_api::authorizationStateWaitCode::ID) {
        prompt_auth_code();
        return;
    }

    if (id == td::td_api::authorizationStateWaitPassword::ID) {
        prompt_password();
        return;
    }

    if (id == td::td_api::authorizationStateLoggingOut::ID) {
        authorized_ = false;
        return;
    }

    if (id == td::td_api::authorizationStateClosing::ID) {
        authorized_ = false;
        return;
    }

    if (id == td::td_api::authorizationStateClosed::ID) {
        authorized_ = false;
        {
            std::lock_guard lock(auth_mutex_);
            auth_failed_ = true;
            auth_error_ = "Authorization closed";
        }
        auth_cv_.notify_all();
        running_ = false;
    }
}

void TelegramBackend::handle_chats(td::td_api::object_ptr<td::td_api::chats> chats) {
    if (chats == nullptr) {
        return;
    }

    std::lock_guard lock(chats_mutex_);
    pending_chat_ids_.clear();
    pending_chats_.clear();
    expected_chat_count_ = chats->chat_ids_.size();

    if (expected_chat_count_ == 0) {
        emit(engine::events::EventId::BackendChatList, pending_chats_);
        return;
    }

    for (std::size_t i = 0; i < chats->chat_ids_.size(); ++i) {
        const auto chat_id = chats->chat_ids_[i];
        pending_chat_ids_.insert(chat_id);
        send_query(td::td_api::make_object<td::td_api::getChat>(chat_id));
    }
}

void TelegramBackend::handle_chat(td::td_api::object_ptr<td::td_api::chat> chat) {
    if (chat == nullptr) {
        return;
    }

    std::lock_guard lock(chats_mutex_);
    if (pending_chat_ids_.find(chat->id_) == pending_chat_ids_.end()) {
        return;
    }

    chat_titles_[chat->id_] = chat->title_;
    pending_chats_.push_back(ChatSummary{chat->id_, chat->title_});

    if (pending_chats_.size() >= expected_chat_count_) {
        emit(engine::events::EventId::BackendChatList, pending_chats_);
        pending_chat_ids_.clear();
        pending_chats_.clear();
        expected_chat_count_ = 0;
    }
}

void TelegramBackend::handle_messages(td::td_api::object_ptr<td::td_api::messages> messages) {
    if (messages == nullptr) {
        return;
    }

    ChatId chat_id = 0;
    std::vector<engine::backend::Message> buffer{};

    for (auto& msg_ptr : messages->messages_) {
        if (msg_ptr == nullptr) {
            continue;
        }

        const auto backend_msg = to_backend_message(*msg_ptr);
        if (!backend_msg.has_value()) {
            continue;
        }

        if (chat_id == 0) {
            chat_id = backend_msg->chat_id;
        }
        buffer.push_back(*backend_msg);
    }

    if (buffer.empty()) {
        return;
    }

    bool should_emit = false;
    ChatHistory aggregated{};

    {
        std::lock_guard lock(history_mutex_);
        auto& pending = pending_history_[chat_id];
        pending.insert(pending.end(), buffer.begin(), buffer.end());

        const auto it_limit = pending_history_limits_.find(chat_id);
        const std::int32_t expected = (it_limit != pending_history_limits_.end()) ? it_limit->second : 0;

        if (expected == 0 || static_cast<std::int32_t>(pending.size()) >= expected) {
            aggregated.chat_id = chat_id;
            aggregated.messages = std::move(pending);
            pending_history_.erase(chat_id);
            pending_history_limits_.erase(chat_id);
            should_emit = true;
        } else {
            const auto from_id = buffer.back().id;
            const auto remaining = expected - static_cast<std::int32_t>(pending.size());
            send_query(td::td_api::make_object<td::td_api::getChatHistory>(
                chat_id,
                from_id,
                -1,
                remaining,
                false
            ));
        }
    }

    if (should_emit) {
        emit(engine::events::EventId::BackendChatHistory, aggregated);
    }
}

void TelegramBackend::handle_new_message(td::td_api::object_ptr<td::td_api::updateNewMessage> update) {
    if (update == nullptr || update->message_ == nullptr) {
        return;
    }

    const auto backend_msg = to_backend_message(*update->message_);
    if (!backend_msg.has_value()) {
        return;
    }

    emit(engine::events::EventId::BackendNewMessage, *backend_msg);
}

auto TelegramBackend::next_request_id() -> std::int64_t {
    return next_query_id_.fetch_add(1);
}

void TelegramBackend::send_query(td::td_api::object_ptr<td::td_api::Function> function, std::int64_t request_id) {
    const auto query_id = request_id != 0 ? request_id : next_request_id();
    std::lock_guard lock(send_mutex_);
    client_manager_.send(client_id_, query_id, std::move(function));
}

void TelegramBackend::send_tdlib_parameters() {
    auto request = td::td_api::make_object<td::td_api::setTdlibParameters>();
    request->use_test_dc_ = false;
    request->database_directory_ = "td_db";
    request->use_message_database_ = true;
    request->use_secret_chats_ = true;
    const auto& cfg = engine::config::ConfigService::ref();
    request->api_id_ = cfg.telegram.api_id;
    request->api_hash_ = cfg.telegram.api_hash;
    request->system_language_code_ = "en";
    request->device_model_ = "lounge";
    request->application_version_ = "0.1";

    if (request->api_id_ == 0 || request->api_hash_.empty()) {
        std::string api_id_input = prompt_line("Enter Telegram api_id: ");
        std::string api_hash_input = prompt_line("Enter Telegram api_hash: ");

        int api_id_value = 0;
        const auto* begin = api_id_input.data();
        const auto* end = api_id_input.data() + api_id_input.size();
        const auto parse_result = std::from_chars(begin, end, api_id_value);
        if (parse_result.ec == std::errc{}) {
            request->api_id_ = api_id_value;
        } else {
            request->api_id_ = 0;
        }
        request->api_hash_ = std::move(api_hash_input);
    }

    if (request->api_id_ != 0 && !request->api_hash_.empty()) {
        [[maybe_unused]] const auto _ = engine::config::ConfigService::set_telegram_credentials(
            request->api_id_,
            request->api_hash_
        );
    }

    send_query(std::move(request));
}

void TelegramBackend::prompt_phone_number() {
    std::string phone;
    while (phone.empty()) {
        phone = prompt_line("Enter phone number (e.g., +15551234567): ");
    }
    send_query(td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(phone, nullptr));
}

void TelegramBackend::prompt_auth_code() {
    std::string code;
    while (code.empty()) {
        code = prompt_line("Enter authentication code: ");
    }
    send_query(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code));
}

void TelegramBackend::prompt_password() {
    std::string password;
    while (password.empty()) {
        password = prompt_line("Enter password: ");
    }
    send_query(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password));
}

auto TelegramBackend::prompt_line(std::string_view label) -> std::string {
    std::cerr << label;
    std::cerr.flush();
    std::string line;
    if (!std::getline(std::cin, line)) {
        std::cin.clear();
    }
    return line;
}

auto TelegramBackend::to_backend_message(const td::td_api::message& message)
    -> std::optional<engine::backend::Message> {
    if (message.content_ == nullptr) {
        return std::nullopt;
    }

    const auto text = extract_text(*message.content_);
    if (!text.has_value()) {
        return std::nullopt;
    }

    engine::backend::Message backend_msg{};
    backend_msg.id = message.id_;
    backend_msg.chat_id = message.chat_id_;
    backend_msg.text = *text;
    backend_msg.timestamp = message.date_;

    std::string sender_label = "unknown";
    if (message.sender_id_ != nullptr) {
        const auto sender_id = message.sender_id_->get_id();
        if (sender_id == td::td_api::messageSenderUser::ID) {
            const auto& sender = static_cast<const td::td_api::messageSenderUser&>(*message.sender_id_);
            sender_label = "user:" + std::to_string(sender.user_id_);
        } else if (sender_id == td::td_api::messageSenderChat::ID) {
            const auto& sender = static_cast<const td::td_api::messageSenderChat&>(*message.sender_id_);
            sender_label = "chat:" + std::to_string(sender.chat_id_);
        }
    }
    backend_msg.sender = std::move(sender_label);

    return backend_msg;
}

auto TelegramBackend::extract_text(const td::td_api::MessageContent& content)
    -> std::optional<std::string> {
    if (content.get_id() != td::td_api::messageText::ID) {
        return std::nullopt;
    }

    const auto& message_text = static_cast<const td::td_api::messageText&>(content);
    if (message_text.text_ == nullptr) {
        return std::nullopt;
    }

    return message_text.text_->text_;
}

}  // namespace engine::backend::telegram



