#include "game/ui/screens/gameplay/gameplay_screen.hpp"

namespace game::ui::gameplay {

GameplayScreen::GameplayScreen(game::GameState& state)
    : state_{&state} {}

auto GameplayScreen::id() const noexcept -> std::string_view {
    return kId;
}

// TODO: when we have built a proper tree of things to render (including the HUD), we'll
// uncomment this and render actual things
// this throws a warning right now, so we dont forget it. additionally, 
// fixing this means we'll also have to fix the SceneRenderer::build_queue function so it renders actual players
// and actors
// think of it like panda3d hierarchy system where one thing is visible and all children should be visible as well
auto GameplayScreen::build() -> engine::ui::UiScreenBuildResult {
    engine::ui::UiElement container{};
    container.tag = "div";
    container.classes = {"screen-gameplay"};

    engine::ui::UiElement instructions{};
    instructions.tag = "div";
    instructions.classes = {"gameplay-instructions"};
    const bool active = state_ != nullptr ? state_->gameplay_active : false;
    instructions.text = active ? "Use WASD to move the square." : "Loading session...";
    container.children.push_back(std::move(instructions));

    engine::ui::UiScreenBuildResult result{};
    result.root = std::move(container);
    result.stylesheets.push_back("game/ui/screens/gameplay/gameplay_screen.rcss");
    return result;
}

auto GameplayScreen::update(float /*dt*/) -> bool {
    return false;
}

}  // namespace game::ui::gameplay


