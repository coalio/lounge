#include "game/ui/ui_service.hpp"

#include <memory>

#include "game/ui/screens/gameplay/gameplay_screen.hpp"
#include "game/ui/screens/start_menu/start_menu_screen.hpp"

#include "game/ui/ui_macros.hpp"

namespace game::ui {

void initialize(engine::ui::UiSystem& ui_system, game::GameState& state) {
    auto& ui_context = ui_system.context();

    ui_context.set_global_styles({
        "game/ui/styles/base.rcss",
        "game/ui/styles/layout.rcss"
    });

    ui_system.load_font("game/ui/styles/fonts/" LOUNGE_FONT_FAMILY ".otf");

    ui_context.register_screen(
        start_menu::StartMenuScreen::kId,
        [&state] {
            return std::make_unique<start_menu::StartMenuScreen>(state);
        }
    );

    ui_context.register_screen(
        gameplay::GameplayScreen::kId,
        [&state] {
            return std::make_unique<gameplay::GameplayScreen>(state);
        }
    );

    ui_context.push_screen(start_menu::StartMenuScreen::kId);
}

}  // namespace game::ui

