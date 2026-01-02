#pragma once

#include <chrono>
#include <string>
#include <variant>
#include <vector>

#include "engine/backend/backend_types.hpp"

namespace engine::events {

enum class EventId : std::uint32_t {
    BackendStatus = 1,
    BackendChatList = 2,
    BackendChatHistory = 3,
    BackendNewMessage = 4
};

using EventPayload = std::variant<
    std::monostate,
    engine::backend::BackendStatus,
    std::vector<engine::backend::ChatSummary>,
    engine::backend::ChatHistory,
    engine::backend::Message>;

struct Event {
    EventId id{EventId::BackendStatus};
    std::string source{};
    std::chrono::steady_clock::time_point timestamp{std::chrono::steady_clock::now()};
    EventPayload payload{};
};

}  // namespace engine::events


