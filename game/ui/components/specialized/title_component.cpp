#include "game/ui/components/specialized/title_component.hpp"

#include <utility>

namespace game::ui::components {

TitleComponent::TitleComponent(std::string text)
    : Component<LabelProps>(LabelProps{
          .text = std::move(text),
          .font_size = 42,
          .variant_class = "title"
      }) {}

auto TitleComponent::render() -> engine::ui::UiElement {
    LabelComponent label(Component<LabelProps>::props());
    return label.render();
}

auto TitleComponent::stylesheets() -> std::vector<std::string> {
    auto sheets = LabelComponent::stylesheets();
    sheets.push_back("game/ui/components/specialized/title_component.rcss");
    return sheets;
}

}  // namespace game::ui::components

