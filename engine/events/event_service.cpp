#include "engine/events/event_service.hpp"

#include <algorithm>
#include <utility>

namespace engine::events {

auto EventService::subscribe(EventId id, Handler handler) -> Subscription {
    const auto token = next_token_.fetch_add(1);
    {
        std::lock_guard lock(handlers_mutex_);
        handlers_[id].push_back(HandlerEntry{token, std::move(handler)});
    }
    return Subscription{id, token};
}

void EventService::unsubscribe(const Subscription& subscription) {
    std::lock_guard lock(handlers_mutex_);
    auto it = handlers_.find(subscription.id);
    if (it == handlers_.end()) {
        return;
    }
    auto& entries = it->second;
    entries.erase(
        std::remove_if(
            entries.begin(),
            entries.end(),
            [&subscription](const HandlerEntry& entry) { return entry.token == subscription.token; }
        ),
        entries.end()
    );
}

void EventService::emit(Event event) {
    std::lock_guard lock(queue_mutex_);
    queue_.push_back(std::move(event));
}

void EventService::dispatch() {
    std::vector<Event> local_queue{};
    {
        std::lock_guard lock(queue_mutex_);
        if (queue_.empty()) {
            return;
        }
        local_queue.swap(queue_);
    }

    for (const auto& event : local_queue) {
        std::vector<HandlerEntry> handlers_copy{};
        {
            std::lock_guard lock(handlers_mutex_);
            auto it = handlers_.find(event.id);
            if (it != handlers_.end()) {
                handlers_copy = it->second;
            }
        }

        for (const auto& entry : handlers_copy) {
            if (entry.handler) {
                entry.handler(event);
            }
        }
    }
}

}  // namespace engine::events


