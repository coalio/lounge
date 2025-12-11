#include "game/ui/components/specialized/menu_option_component.hpp"

#include <utility>

namespace game::ui::components {

MenuOptionComponent::MenuOptionComponent(MenuOptionProps props)
    : Component<MenuOptionProps>(std::move(props)) {}

auto MenuOptionComponent::render() -> engine::ui::UiElement {
    const auto& data = Component<MenuOptionProps>::props();
    LabelProps label_props{
        .id = data.id,
        .text = data.label,
        .font_size = 28,
        .variant_class = "option"
    };

    if (data.on_select) {
        label_props.on_click = data.on_select;
    }

    LabelComponent label(std::move(label_props));
    return label.render();
}

auto MenuOptionComponent::stylesheets() -> std::vector<std::string> {
    auto sheets = LabelComponent::stylesheets();
    sheets.push_back("game/ui/components/specialized/menu_option_component.rcss");
    return sheets;
}

}  // namespace game::ui::components

