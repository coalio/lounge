#include "game/pipeline/stages/render_stage.hpp"

#include "engine/render/renderer.hpp"
#include "engine/ui/ui_system.hpp"
#include "game/pipeline/game_pipeline.hpp"
#include "game/render/scene_renderer.hpp"

namespace game::pipeline::stages {

void RenderStage::run(GameContext& ctx) {
    ctx.renderer.begin_frame();
    ctx.scene_renderer.build_queue(ctx.game_state, ctx.render_queue);
    ctx.renderer.flush(ctx.render_queue);
    ctx.ui_system.render();
    ctx.renderer.end_frame();
}

}  // namespace game::pipeline::stages

