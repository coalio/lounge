#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <expected>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>

#include "engine/backend/backend.hpp"
#include "engine/backend/backend_types.hpp"

namespace engine::backend {

class NetworkManager {
public:
    explicit NetworkManager(std::unique_ptr<Backend> backend);
    NetworkManager(const NetworkManager&) = delete;
    auto operator=(const NetworkManager&) -> NetworkManager& = delete;
    NetworkManager(NetworkManager&&) = delete;
    auto operator=(NetworkManager&&) -> NetworkManager& = delete;
    ~NetworkManager();

    auto start() -> std::expected<void, std::string>;
    void stop();

    void request_chats(std::int32_t limit);
    void request_history(ChatId chat_id, std::int32_t limit);
    void send_message(ChatId chat_id, std::string_view text);

private:
    enum class CommandType { RequestChats, RequestHistory, SendMessage, Shutdown };

    struct Command {
        CommandType type{CommandType::RequestChats};
        ChatId chat_id{0};
        std::int32_t limit{0};
        std::string text{};
    };

    [[nodiscard]] static auto make_shutdown_command() -> Command;
    void worker_loop();
    void enqueue(Command cmd);
    auto dequeue() -> Command;

    std::unique_ptr<Backend> backend_{};
    std::thread worker_{};
    std::mutex mutex_{};
    std::condition_variable cv_{};
    std::deque<Command> commands_{};
    std::atomic<bool> running_{false};
    std::atomic<bool> started_{false};
};

}  // namespace engine::backend


