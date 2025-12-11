#pragma once

#include <vector>

#include "engine/ui/ui_component.hpp"
#include "game/ui/components/specialized/menu_option_component.hpp"

namespace game::ui::components {

struct OptionListProps {
    std::vector<MenuOptionProps> options{};
};

class OptionListComponent : public engine::ui::Component<OptionListProps> {
public:
    explicit OptionListComponent(OptionListProps props);

    auto render() -> engine::ui::UiElement override;
    [[nodiscard]] static auto stylesheets() -> std::vector<std::string>;
};

}  // namespace game::ui::components

