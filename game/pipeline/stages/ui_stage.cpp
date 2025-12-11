#include "game/pipeline/stages/ui_stage.hpp"

#include "engine/ui/ui_system.hpp"
#include "game/pipeline/game_pipeline.hpp"

namespace game::pipeline::stages {

void UiStage::run(GameContext& ctx) {
    ctx.ui_system.update(ctx.dt);
}

}  // namespace game::pipeline::stages

