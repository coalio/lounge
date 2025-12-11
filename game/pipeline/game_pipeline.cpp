#include "game/pipeline/game_pipeline.hpp"

namespace game::pipeline {

GamePipeline::GamePipeline() {
    pipeline_.add_stage([this](GameContext& ctx) { input_stage_.run(ctx); });
    pipeline_.add_stage([this](GameContext& ctx) { logic_stage_.run(ctx); });
    pipeline_.add_stage([this](GameContext& ctx) { ui_stage_.run(ctx); });
    pipeline_.add_stage([this](GameContext& ctx) { render_stage_.run(ctx); });
}

void GamePipeline::run(GameContext& ctx) const {
    pipeline_.run(ctx);
}

}  // namespace game::pipeline


