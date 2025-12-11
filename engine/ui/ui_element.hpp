#pragma once

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "engine/ui/ui_types.hpp"

namespace engine::ui {

using UiEventHandler = std::function<void()>;

struct UiEventBinding {
    std::string id{};
    std::string type{};
    UiEventHandler handler{};
};

struct UiElementEvent {
    std::string type{};
    UiEventHandler handler{};
};

struct UiElement {
    std::string tag{};
    std::string id{};
    std::vector<std::string> classes{};
    std::vector<std::pair<std::string, std::string>> attributes{};
    std::optional<std::string> text{};
    std::vector<UiElementEvent> events{};
    std::vector<UiElement> children{};
};

}  // namespace engine::ui

