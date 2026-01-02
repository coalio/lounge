#pragma once

#include <optional>
#include <vector>

#include "engine/backend/backend_types.hpp"

namespace game::state {

struct ChatState {
    bool backend_ready{false};
    bool backend_connecting{false};
    std::optional<engine::backend::ChatId> selected_chat{};
    std::vector<engine::backend::ChatSummary> chats{};
    std::vector<engine::backend::Message> chat_history{};
};

}  // namespace game::state


