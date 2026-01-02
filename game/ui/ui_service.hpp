#pragma once

#include "engine/backend/network_manager.hpp"
#include "engine/ui/ui_system.hpp"
#include "game/state.hpp"
#include "game/state/chat_store.hpp"

namespace game::ui {

void initialize(engine::ui::UiSystem& ui_system,
                game::GameState& state,
                engine::backend::NetworkManager& network_manager,
                game::state::ChatStore& chat_store);

}  // namespace game::ui

