#pragma once

#include <variant>
#include <vector>

#include "engine/backend/backend_types.hpp"

namespace game::state {

struct SetBackendStatus {
    engine::backend::BackendStatus status{};
};

struct SetChats {
    std::vector<engine::backend::ChatSummary> chats{};
};

struct SetChatHistory {
    engine::backend::ChatHistory history{};
};

struct AppendMessage {
    engine::backend::Message message{};
};

struct ResetChats {};

using ChatAction = std::variant<SetBackendStatus, SetChats, SetChatHistory, AppendMessage, ResetChats>;

}  // namespace game::state


