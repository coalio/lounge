#include "game/pipeline/stages/logic_stage.hpp"

#include <cmath>

#include "engine/input/input_state.hpp"
#include "game/pipeline/game_pipeline.hpp"
#include "game/state.hpp"

namespace game::pipeline::stages {

void LogicStage::run(GameContext& ctx) {
    if (!ctx.game_state.gameplay_active) {
        return;
    }

    float dx = 0.0F;
    float dy = 0.0F;

    if (ctx.input_state.left) {
        dx -= 1.0F;
    }

    if (ctx.input_state.right) {
        dx += 1.0F;
    }

    if (ctx.input_state.up) {
        dy -= 1.0F;
    }

    if (ctx.input_state.down) {
        dy += 1.0F;
    }

    if (dx != 0.0F || dy != 0.0F) {
        const float len = std::sqrt(dx * dx + dy * dy);

        if (len > 0.0F) {
            dx /= len;
            dy /= len;
        }

        ctx.game_state.player_pos.x += dx * ctx.game_state.player_speed * ctx.dt;
        ctx.game_state.player_pos.y += dy * ctx.game_state.player_speed * ctx.dt;
    }
}

}  // namespace game::pipeline::stages

