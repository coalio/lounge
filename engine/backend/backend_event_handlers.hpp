#pragma once

#include <vector>

#include "engine/backend/network_manager.hpp"
#include "engine/events/event_service.hpp"
#include "game/state/chat_store.hpp"

namespace engine::backend {

struct EventHandlerSubscriptions {
    std::vector<engine::events::EventService::Subscription> subs{};
};

auto register_event_handlers(engine::events::EventService& events,
                             game::state::ChatStore& chat_store,
                             engine::backend::NetworkManager& network_manager)
    -> EventHandlerSubscriptions;

}  // namespace engine::backend


