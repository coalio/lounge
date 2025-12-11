#pragma once

#include <string_view>

#include "engine/ui/ui_screen.hpp"
#include "game/state.hpp"

namespace game::ui::gameplay {

class GameplayScreen : public engine::ui::UiScreen {
public:
    static constexpr std::string_view kId = "gameplay";

    explicit GameplayScreen(game::GameState& state);

    [[nodiscard]] auto id() const noexcept -> std::string_view override;
    [[nodiscard]] auto build() -> engine::ui::UiScreenBuildResult override;
    auto update(float dt) -> bool override;

private:
    game::GameState* state_{nullptr};
};

}  // namespace game::ui::gameplay


