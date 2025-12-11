#pragma once

#include <functional>
#include <string>
#include <vector>

#include "game/ui/components/base/label_component.hpp"

namespace game::ui::components {

struct MenuOptionProps {
    std::string id{};
    std::string label{};
    std::function<void()> on_select{};
};

class MenuOptionComponent : public engine::ui::Component<MenuOptionProps> {
public:
    explicit MenuOptionComponent(MenuOptionProps props);

    auto render() -> engine::ui::UiElement override;
    [[nodiscard]] static auto stylesheets() -> std::vector<std::string>;
};

}  // namespace game::ui::components

