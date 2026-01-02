#include "engine/backend/backend_event_handlers.hpp"

#include <algorithm>
#include <iostream>
#include <utility>

namespace engine::backend {

auto register_event_handlers(engine::events::EventService& events,
                             game::state::ChatStore& chat_store,
                             engine::backend::NetworkManager& network_manager)
    -> EventHandlerSubscriptions {
    EventHandlerSubscriptions holder{};

    holder.subs.push_back(events.subscribe(
        engine::events::EventId::BackendStatus,
        [&chat_store, &network_manager](const engine::events::Event& event) {
            const auto* payload = std::get_if<engine::backend::BackendStatus>(&event.payload);
            if (payload == nullptr) {
                return;
            }
            chat_store.dispatch(game::state::SetBackendStatus{*payload});
        }
    ));

    holder.subs.push_back(events.subscribe(
        engine::events::EventId::BackendChatList,
        [&chat_store](const engine::events::Event& event) {
            const auto* payload = std::get_if<std::vector<engine::backend::ChatSummary>>(&event.payload);
            if (payload == nullptr) {
                return;
            }
            const auto take_count = std::min<std::size_t>(5, payload->size());
            std::vector<engine::backend::ChatSummary> limited(payload->begin(), payload->begin() + take_count);
            chat_store.dispatch(game::state::SetChats{std::move(limited)});

            const auto snapshot = chat_store.state();
            std::cout << "BackendChatList event received (" << snapshot.chats.size() << " entries):" << std::endl;
            for (const auto& chat : snapshot.chats) {
                std::cout << "  chat_id=" << chat.id << " title=\"" << chat.title << "\"" << std::endl;
            }
        }
    ));

    holder.subs.push_back(events.subscribe(
        engine::events::EventId::BackendChatHistory,
        [&chat_store](const engine::events::Event& event) {
            const auto* history = std::get_if<engine::backend::ChatHistory>(&event.payload);
            if (history == nullptr) {
                return;
            }
            chat_store.dispatch(game::state::SetChatHistory{*history});
            std::cout << "Chat history for chat " << history->chat_id << " ("
                      << history->messages.size() << " messages):" << std::endl;
            for (const auto& message : history->messages) {
                std::cout << "[" << message.sender << "] " << message.text << std::endl;
            }
        }
    ));

    holder.subs.push_back(events.subscribe(
        engine::events::EventId::BackendNewMessage,
        [&chat_store](const engine::events::Event& event) {
            const auto* message = std::get_if<engine::backend::Message>(&event.payload);
            if (message == nullptr) {
                return;
            }
            chat_store.dispatch(game::state::AppendMessage{*message});
        }
    ));

    return holder;
}

}  // namespace engine::backend


