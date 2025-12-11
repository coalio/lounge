#pragma once

#include "engine/core/types.hpp"

namespace game {

inline constexpr float PLAYER_SIZE = 32.0F;
inline constexpr float PLAYER_SPEED = 200.0F;

struct GameState {
    engine::core::Vec2 player_pos{};
    float player_size{::game::PLAYER_SIZE};
    float player_speed{::game::PLAYER_SPEED};
    bool gameplay_active{false};
};

}  // namespace game


