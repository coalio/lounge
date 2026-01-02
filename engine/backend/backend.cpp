#include "engine/backend/backend.hpp"

#include <utility>

namespace engine::backend {

void Backend::emit(engine::events::EventId id, engine::events::EventPayload payload) {
    if (events_ == nullptr) {
        return;
    }

    engine::events::Event event{};
    event.id = id;
    event.source = source_;
    event.payload = std::move(payload);
    events_->emit(std::move(event));
}

}  // namespace engine::backend


