#include "game/ui/components/specialized/option_list_component.hpp"

#include <utility>

namespace game::ui::components {

OptionListComponent::OptionListComponent(OptionListProps props)
    : Component<OptionListProps>(std::move(props)) {}

auto OptionListComponent::render() -> engine::ui::UiElement {
    engine::ui::UiElement list{};
    list.tag = "div";
    list.id = "option-list";
    list.classes = {"option-list"};

    for (const auto& option : Component<OptionListProps>::props().options) {
        MenuOptionComponent component(option);
        list.children.push_back(component.render());
    }

    return list;
}

auto OptionListComponent::stylesheets() -> std::vector<std::string> {
    return {
        "game/ui/components/specialized/option_list_component.rcss"
    };
}

}  // namespace game::ui::components

