#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "engine/ui/ui_element.hpp"

namespace engine::ui {

struct UiDocument {
    std::string markup{};
    std::vector<UiEventBinding> events{};
};

auto build_ui_document(UiElement root,
                       std::span<const std::string_view> stylesheets,
                       std::string_view template_markup) -> UiDocument;

}  // namespace engine::ui

