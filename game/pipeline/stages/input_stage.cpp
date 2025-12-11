#include "game/pipeline/stages/input_stage.hpp"

#include "engine/input/input_handler.hpp"
#include "engine/input/input_state.hpp"
#include "engine/ui/ui_system.hpp"
#include "game/pipeline/game_pipeline.hpp"

namespace game::pipeline::stages {

void InputStage::run(GameContext& ctx) {
    ctx.input_handler.update(ctx.input_state);

    for (const auto& event : ctx.input_handler.events()) {
        ctx.ui_system.process_event(event);
    }

    if (ctx.input_state.quit) {
        ctx.running = false;
    }
}

}  // namespace game::pipeline::stages

