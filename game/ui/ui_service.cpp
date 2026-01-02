#include "game/ui/ui_service.hpp"

#include <memory>

#include "game/ui/screens/gameplay/gameplay_screen.hpp"
#include "game/ui/screens/join_friend/join_friend_screen.hpp"
#include "game/ui/screens/start_menu/start_menu_screen.hpp"

#include "game/ui/ui_macros.hpp"

namespace game::ui {

void initialize(engine::ui::UiSystem& ui_system,
                game::GameState& state,
                engine::backend::NetworkManager& network_manager,
                game::state::ChatStore& chat_store) {
    auto& ui_context = ui_system.context();

    ui_context.set_global_styles({
        "game/ui/styles/base.rcss",
        "game/ui/styles/layout.rcss"
    });

    ui_system.load_font("game/ui/styles/fonts/" LOUNGE_FONT_FAMILY ".otf");

    ui_context.register_screen(
        start_menu::StartMenuScreen::kId,
        [&state, &chat_store] {
            return std::make_unique<start_menu::StartMenuScreen>(state, chat_store);
        }
    );

    ui_context.register_screen(
        join_friend::JoinFriendScreen::kId,
        [&state, &network_manager, &chat_store] {
            return std::make_unique<join_friend::JoinFriendScreen>(state, network_manager, chat_store);
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

