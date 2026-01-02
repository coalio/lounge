#pragma once

#include <string_view>
#include <vector>

#include "engine/ui/ui_screen.hpp"
#include "game/state/chat_store.hpp"
#include "game/state.hpp"
#include "game/ui/components/specialized/option_list_component.hpp"
#include "game/ui/components/specialized/title_component.hpp"

namespace game::ui::start_menu {

class StartMenuScreen : public engine::ui::UiScreen {
public:
    static constexpr std::string_view kId = "start_menu";

    StartMenuScreen(game::GameState& state, game::state::ChatStore& chat_store);

    [[nodiscard]] auto id() const noexcept -> std::string_view override;
    [[nodiscard]] auto build() -> engine::ui::UiScreenBuildResult override;
    void on_attach(engine::ui::UiScreenHost& host) override;
    void on_detach() override;

private:
    void handle_join_friend();
    [[nodiscard]] auto build_options() -> std::vector<game::ui::components::MenuOptionProps>;

    game::GameState* state_{nullptr};
    game::state::ChatStore* chat_store_{nullptr};
    engine::ui::UiScreenHost* host_{nullptr};
};

}  // namespace game::ui::start_menu

