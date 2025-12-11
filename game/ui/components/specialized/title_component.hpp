#pragma once

#include <string>
#include <vector>

#include "game/ui/components/base/label_component.hpp"

namespace game::ui::components {

class TitleComponent : public engine::ui::Component<LabelProps> {
public:
    explicit TitleComponent(std::string text);

    auto render() -> engine::ui::UiElement override;
    [[nodiscard]] static auto stylesheets() -> std::vector<std::string>;
};

}  // namespace game::ui::components

