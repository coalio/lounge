#pragma once

#include <string_view>
#include <vector>

#include "engine/backend/network_manager.hpp"
#include "engine/ui/ui_screen.hpp"
#include "game/state/chat_store.hpp"
#include "game/state.hpp"
#include "game/ui/components/base/label_component.hpp"
#include "game/ui/components/specialized/option_list_component.hpp"
#include "game/ui/components/specialized/title_component.hpp"

namespace game::ui::join_friend {

class JoinFriendScreen : public engine::ui::UiScreen {
public:
    static constexpr std::string_view kId = "join_friend";

    JoinFriendScreen(game::GameState& state,
                     engine::backend::NetworkManager& network_manager,
                     game::state::ChatStore& chat_store);

    [[nodiscard]] auto id() const noexcept -> std::string_view override;
    [[nodiscard]] auto build() -> engine::ui::UiScreenBuildResult override;
    void on_attach(engine::ui::UiScreenHost& host) override;
    void on_detach() override;
    auto update(float dt) -> bool override;

private:
    [[nodiscard]] auto build_options() -> std::vector<game::ui::components::MenuOptionProps>;
    [[nodiscard]] auto build_status_label() const -> engine::ui::UiElement;
    void handle_select_chat(engine::backend::ChatId chat_id);
    void handle_back();

    game::GameState* state_{nullptr};
    engine::backend::NetworkManager* network_manager_{nullptr};
    game::state::ChatStore* chat_store_{nullptr};
    engine::ui::UiScreenHost* host_{nullptr};
    std::size_t last_chat_count_{0};
    std::size_t chat_subscription_{0};
    bool dirty_{false};
};

}  // namespace game::ui::join_friend


