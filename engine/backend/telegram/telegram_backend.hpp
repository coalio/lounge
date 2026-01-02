#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include "engine/backend/backend.hpp"

namespace engine::backend::telegram {

class TelegramBackend : public Backend {
public:
    explicit TelegramBackend(engine::events::EventService& events, std::string source);
    TelegramBackend(const TelegramBackend&) = delete;
    auto operator=(const TelegramBackend&) -> TelegramBackend& = delete;
    TelegramBackend(TelegramBackend&&) = delete;
    auto operator=(TelegramBackend&&) -> TelegramBackend& = delete;
    ~TelegramBackend() override;

    auto start() -> std::expected<void, std::string> override;
    void stop() override;
    void request_chats(std::int32_t limit) override;
    void request_history(ChatId chat_id, std::int32_t limit) override;
    void send_message(ChatId chat_id, std::string_view text) override;

private:
    using TdObject = td::td_api::object_ptr<td::td_api::Object>;

    void receiver_loop();
    void process_response(td::ClientManager::Response response);
    void handle_update(TdObject update);
    void handle_response(std::int64_t request_id, TdObject object);

    void handle_authorization_state(td::td_api::object_ptr<td::td_api::AuthorizationState> state);
    void handle_chats(td::td_api::object_ptr<td::td_api::chats> chats);
    void handle_chat(td::td_api::object_ptr<td::td_api::chat> chat);
    void handle_messages(td::td_api::object_ptr<td::td_api::messages> messages);
    void handle_new_message(td::td_api::object_ptr<td::td_api::updateNewMessage> update);

    [[nodiscard]] auto next_request_id() -> std::int64_t;
    void send_query(td::td_api::object_ptr<td::td_api::Function> function, std::int64_t request_id = 0);

    void send_tdlib_parameters();

    // TODO: these are for debugging purposes only, remove later
    void prompt_phone_number();
    void prompt_auth_code();
    void prompt_password();

    [[nodiscard]] auto prompt_line(std::string_view label) -> std::string;

    [[nodiscard]] auto to_backend_message(const td::td_api::message& message)
        -> std::optional<engine::backend::Message>;
    [[nodiscard]] auto extract_text(const td::td_api::MessageContent& content)
        -> std::optional<std::string>;

    td::ClientManager client_manager_{};
    std::int64_t client_id_{0};
    std::atomic<std::int64_t> next_query_id_{1};

    std::atomic<bool> running_{false};
    std::atomic<bool> authorized_{false};
    std::thread receiver_thread_{};
    std::mutex send_mutex_{};

    std::mutex auth_mutex_{};
    std::condition_variable auth_cv_{};
    bool auth_ready_{false};
    bool auth_failed_{false};
    std::string auth_error_{};

    std::unordered_map<std::int64_t, std::string> chat_titles_{};
    std::unordered_set<std::int64_t> pending_chat_ids_{};
    std::vector<engine::backend::ChatSummary> pending_chats_{};
    std::size_t expected_chat_count_{0};
    std::mutex chats_mutex_{};

    std::mutex history_mutex_{};
    std::unordered_map<ChatId, std::vector<engine::backend::Message>> pending_history_{};
    std::unordered_map<ChatId, std::int32_t> pending_history_limits_{};
};

}  // namespace engine::backend::telegram



