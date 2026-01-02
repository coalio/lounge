#pragma once

#include <expected>
#include <string>
#include <string_view>

#include "engine/backend/backend_types.hpp"
#include "engine/events/event.hpp"
#include "engine/events/event_service.hpp"

namespace engine::backend {

class Backend {
public:
    explicit Backend(engine::events::EventService& events, std::string source);
    Backend(const Backend&) = delete;
    auto operator=(const Backend&) -> Backend& = delete;
    Backend(Backend&&) = delete;
    auto operator=(Backend&&) -> Backend& = delete;
    virtual ~Backend() = default;

    virtual auto start() -> std::expected<void, std::string> = 0;
    virtual void stop() = 0;
    virtual void request_chats(std::int32_t limit) = 0;
    virtual void request_history(ChatId chat_id, std::int32_t limit) = 0;
    virtual void send_message(ChatId chat_id, std::string_view text) = 0;

protected:
    void emit(engine::events::EventId id, engine::events::EventPayload payload);

private:
    engine::events::EventService* events_{nullptr};
    std::string source_{};
};

inline Backend::Backend(engine::events::EventService& events, std::string source)
    : events_{&events},
      source_{std::move(source)} {}

}  // namespace engine::backend

