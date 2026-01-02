#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "engine/events/event.hpp"

namespace engine::events {

class EventService {
public:
    using Handler = std::function<void(const Event&)>;

    struct Subscription {
        EventId id{EventId::BackendStatus};
        std::size_t token{0};
    };

    EventService() = default;
    EventService(const EventService&) = delete;
    auto operator=(const EventService&) -> EventService& = delete;
    EventService(EventService&&) = delete;
    auto operator=(EventService&&) -> EventService& = delete;
    ~EventService() = default;

    [[nodiscard]] auto subscribe(EventId id, Handler handler) -> Subscription;
    void unsubscribe(const Subscription& subscription);

    void emit(Event event);
    void dispatch();

private:
    struct HandlerEntry {
        std::size_t token{0};
        Handler handler{};
    };

    std::mutex handlers_mutex_{};
    std::unordered_map<EventId, std::vector<HandlerEntry>> handlers_{};
    std::atomic<std::size_t> next_token_{1};

    std::mutex queue_mutex_{};
    std::vector<Event> queue_{};
};

}  // namespace engine::events


