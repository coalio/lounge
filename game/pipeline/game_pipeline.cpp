#include "game/pipeline/game_pipeline.hpp"

#include "engine/input/input_handler.hpp"
#include "engine/input/input_state.hpp"
#include "engine/render/render_queue.hpp"
#include "engine/core/types.hpp"
#include "game/render/scene_renderer.hpp"
#include "game/state.hpp"

#include <cmath>

namespace game::pipeline {

namespace {

inline void input_stage(GameContext& ctx) {
    ctx.input_handler.update(ctx.input_state);

    if (ctx.input_state.quit) {
        ctx.running = false;
    }
}

inline void logic_stage(GameContext& ctx) {
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

inline void render_stage(GameContext& ctx) {
    ctx.scene_renderer.build_queue(ctx.game_state, ctx.render_queue);
}

}  // namespace

GamePipeline::GamePipeline() {
    pipeline_.add_stage(input_stage);
    pipeline_.add_stage(logic_stage);
    pipeline_.add_stage(render_stage);
}

void GamePipeline::run(GameContext& ctx) const {
    pipeline_.run(ctx);
}

}  // namespace game::pipeline


