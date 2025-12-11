#include "game/ui/components/base/label_component.hpp"

#include <utility>

namespace game::ui::components {

LabelComponent::LabelComponent(LabelProps props)
    : Component<LabelProps>(std::move(props)) {}

auto LabelComponent::render() -> engine::ui::UiElement {
    const auto& data = Component<LabelProps>::props();

    engine::ui::UiElement element{};
    element.tag = data.on_click.has_value() ? "button" : "div";
    element.id = data.id;
    element.classes.push_back("label");
    if (!data.variant_class.empty()) {
        element.classes.push_back(data.variant_class);
    }

    const std::string style = "font-size: " + std::to_string(data.font_size) + "px;";
    element.attributes.emplace_back("style", style);
    element.text = data.text;

    if (data.on_click.has_value()) {
        element.events.push_back(engine::ui::UiElementEvent{
            .type = "click",
            .handler = *data.on_click
        });
    }

    return element;
}

auto LabelComponent::stylesheets() -> std::vector<std::string> {
    return {
        "game/ui/components/base/label_component.rcss"
    };
}

}  // namespace game::ui::components

