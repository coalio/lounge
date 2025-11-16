#include "engine/platform/sdl_platform.hpp"
#include "engine/render/renderer.hpp"
#include "engine/resources/resource_manager.hpp"

#include "engine/input/input_handler.hpp"
#include "engine/input/input_state.hpp"
#include "engine/render/render_queue.hpp"

#include "game/pipeline/game_pipeline.hpp"
#include "game/render/scene_renderer.hpp"
#include "game/state.hpp"

namespace game {

auto run_game(engine::platform::SdlPlatform& platform,
              engine::render::Renderer& renderer,
              engine::resources::ResourceManager&) -> void {
    engine::input::InputHandler input_handler{};
    engine::input::InputState input_state{};

    GameState state{};
    state.player_pos.x = static_cast<float>(platform.width()) * 0.5F;
    state.player_pos.y = static_cast<float>(platform.height()) * 0.5F;

    engine::render::RenderQueue render_queue{};
    game::render::SceneRenderer scene_renderer{};

    game::pipeline::GamePipeline pipeline{};
    game::pipeline::GameContext ctx{
        platform,
        input_handler,
        input_state,
        state,
        render_queue,
        scene_renderer,
        0.0F,
        true
    };

    while (ctx.running) {
        ctx.dt = platform.compute_delta_seconds();

        pipeline.run(ctx);

        renderer.begin_frame();
        renderer.flush(render_queue);
        renderer.end_frame();
    }
}

}  // namespace game


