#pragma once

#include <optional>
#include <string>
#include <vector>

#include "engine/ui/ui_component.hpp"

namespace game::ui::components {

struct LabelProps {
    std::string id{};
    std::string text{};
    int font_size{24};
    std::string variant_class{};
    std::optional<engine::ui::UiEventHandler> on_click{};
};

class LabelComponent : public engine::ui::Component<LabelProps> {
public:
    explicit LabelComponent(LabelProps props);

    auto render() -> engine::ui::UiElement override;

    [[nodiscard]] static auto stylesheets() -> std::vector<std::string>;
};

}  // namespace game::ui::components

