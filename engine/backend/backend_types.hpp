#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace engine::backend {

using ChatId = std::int64_t;
using MessageId = std::int64_t;
using Timestamp = std::int64_t;

struct ChatSummary {
    ChatId id{0};
    std::string title{};
};

struct Message {
    MessageId id{0};
    ChatId chat_id{0};
    std::string sender{};
    std::string text{};
    Timestamp timestamp{0};
};

struct ChatHistory {
    ChatId chat_id{0};
    std::vector<Message> messages{};
};

enum class BackendStatusKind { Connecting, Ready, Error, Stopped };

struct BackendStatus {
    BackendStatusKind kind{BackendStatusKind::Connecting};
    std::string detail{};
};

enum class BackendEventType { Status, ChatList, ChatHistory, NewMessage };

struct BackendEvent {
    BackendEventType type{BackendEventType::Status};
    std::variant<BackendStatus, std::vector<ChatSummary>, ChatHistory, Message> payload{};
};

}  // namespace engine::backend


