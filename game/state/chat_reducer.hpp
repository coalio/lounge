#pragma once

#include "game/state/chat_actions.hpp"
#include "game/state/chat_state.hpp"

namespace game::state {

inline auto reduce_chat_state(const ChatState& state, const ChatAction& action) -> ChatState {
    ChatState next = state;

    std::visit(
        [&](auto&& act) {
            using T = std::decay_t<decltype(act)>;
            if constexpr (std::is_same_v<T, SetBackendStatus>) {
                next.backend_connecting = act.status.kind == engine::backend::BackendStatusKind::Connecting;
                next.backend_ready = act.status.kind == engine::backend::BackendStatusKind::Ready;
                if (act.status.kind != engine::backend::BackendStatusKind::Ready) {
                    next.chats.clear();
                    next.chat_history.clear();
                    next.selected_chat.reset();
                }
            } else if constexpr (std::is_same_v<T, SetChats>) {
                next.chats = act.chats;
            } else if constexpr (std::is_same_v<T, SetChatHistory>) {
                next.selected_chat = act.history.chat_id;
                next.chat_history = act.history.messages;
            } else if constexpr (std::is_same_v<T, AppendMessage>) {
                if (next.selected_chat == act.message.chat_id) {
                    next.chat_history.push_back(act.message);
                }
            } else if constexpr (std::is_same_v<T, ResetChats>) {
                next.chats.clear();
                next.chat_history.clear();
                next.selected_chat.reset();
                next.backend_connecting = false;
                next.backend_ready = false;
            }
        },
        action
    );

    return next;
}

}  // namespace game::state


