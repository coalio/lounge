#pragma once

#include "engine/store/store.hpp"
#include "game/state/chat_actions.hpp"
#include "game/state/chat_reducer.hpp"
#include "game/state/chat_state.hpp"

namespace game::state {

using ChatStore = engine::store::Store<ChatState, ChatAction>;

}  // namespace game::state


