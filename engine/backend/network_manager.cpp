#include "engine/backend/network_manager.hpp"

#include <utility>

namespace engine::backend {

auto NetworkManager::make_shutdown_command() -> NetworkManager::Command {
    NetworkManager::Command cmd{};
    cmd.type = NetworkManager::CommandType::Shutdown;
    return cmd;
}

NetworkManager::NetworkManager(std::unique_ptr<Backend> backend)
    : backend_{std::move(backend)} {}

NetworkManager::~NetworkManager() {
    stop();
}

auto NetworkManager::start() -> std::expected<void, std::string> {
    if (started_) {
        return {};
    }

    if (backend_ == nullptr) {
        return std::unexpected(std::string{"backend not configured"});
    }

    running_ = true;
    started_ = true;
    worker_ = std::thread(&NetworkManager::worker_loop, this);

    return {};
}

void NetworkManager::stop() {
    if (!started_) {
        return;
    }

    running_ = false;
    {
        std::lock_guard lock(mutex_);
        commands_.push_back(make_shutdown_command());
    }
    cv_.notify_all();

    if (worker_.joinable()) {
        worker_.join();
    }
}

void NetworkManager::request_chats(std::int32_t limit) {
    enqueue(Command{.type = CommandType::RequestChats, .limit = limit});
}

void NetworkManager::request_history(ChatId chat_id, std::int32_t limit) {
    enqueue(Command{
        .type = CommandType::RequestHistory,
        .chat_id = chat_id,
        .limit = limit
    });
}

void NetworkManager::send_message(ChatId chat_id, std::string_view text) {
    enqueue(Command{
        .type = CommandType::SendMessage,
        .chat_id = chat_id,
        .text = std::string{text}
    });
}

void NetworkManager::enqueue(Command cmd) {
    if (!running_) {
        return;
    }

    {
        std::lock_guard lock(mutex_);
        commands_.push_back(std::move(cmd));
    }

    cv_.notify_one();
}

auto NetworkManager::dequeue() -> Command {
    std::unique_lock lock(mutex_);
    cv_.wait(lock, [this] { return !commands_.empty() || !running_; });

    if (commands_.empty()) {
        return make_shutdown_command();
    }

    auto cmd = std::move(commands_.front());
    commands_.pop_front();
    return cmd;
}

void NetworkManager::worker_loop() {
    const auto start_result = backend_->start();
    if (!start_result.has_value()) {
        running_ = false;
        return;
    }

    while (running_) {
        const auto cmd = dequeue();

        if (cmd.type == CommandType::Shutdown) {
            break;
        }

        if (cmd.type == CommandType::RequestChats) {
            backend_->request_chats(cmd.limit);
            continue;
        }

        if (cmd.type == CommandType::RequestHistory) {
            backend_->request_history(cmd.chat_id, cmd.limit);
            continue;
        }

        if (cmd.type == CommandType::SendMessage) {
            backend_->send_message(cmd.chat_id, cmd.text);
            continue;
        }
    }

    backend_->stop();
}

}  // namespace engine::backend


