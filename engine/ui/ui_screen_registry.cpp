#include "engine/ui/ui_screen_registry.hpp"

#include <utility>

namespace engine::ui {

void UiScreenRegistry::register_screen(std::string_view id, Factory factory) {
    factories_.insert_or_assign(std::string{id}, std::move(factory));
}

auto UiScreenRegistry::create(std::string_view id) const -> UiScreenPtr {
    if (const auto it = factories_.find(std::string{id}); it != factories_.end()) {
        return it->second();
    }
    return nullptr;
}

}  // namespace engine::ui

