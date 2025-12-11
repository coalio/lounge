#include "engine/config/config.hpp"
#include "engine/platform/sdl_platform.hpp"
#include "engine/render/renderer.hpp"
#include "engine/resources/resource_manager.hpp"
#include "engine/ui/ui_system.hpp"

#include "engine/input/input_handler.hpp"
#include "engine/input/input_state.hpp"
#include "engine/render/render_queue.hpp"

#include "game/pipeline/game_pipeline.hpp"
#include "game/render/scene_renderer.hpp"
#include "game/state.hpp"
#include "game/ui/ui_service.hpp"

namespace game {

auto run_game(engine::platform::SdlPlatform& platform,
              engine::render::Renderer& renderer,
              engine::resources::ResourceManager& resources) -> void {
    engine::input::InputHandler input_handler{};
    engine::input::InputState input_state{};

    GameState state{};
    const auto& config = engine::config::ConfigService::ref();
    state.player_pos.x = static_cast<float>(config.render.target_width) * 0.5F;
    state.player_pos.y = static_cast<float>(config.render.target_height) * 0.5F;

    engine::render::RenderQueue render_queue{};
    game::render::SceneRenderer scene_renderer{};
    engine::ui::UiSystem ui_system{platform, renderer, resources, config.render};
    game::ui::initialize(ui_system, state);

    game::pipeline::GamePipeline pipeline{};
    game::pipeline::GameContext ctx{
        platform,
        input_handler,
        input_state,
        state,
        render_queue,
        scene_renderer,
        renderer,
        ui_system,
        0.0F,
        true
    };

    while (ctx.running) {
        ctx.dt = platform.compute_delta_seconds();

        pipeline.run(ctx);
    }
}

}  // namespace game


