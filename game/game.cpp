#include <iostream>

#include "engine/backend/backend_event_handlers.hpp"
#include "engine/backend/network_manager.hpp"
#include "game/state/chat_store.hpp"
#include "engine/backend/telegram/telegram_backend.hpp"
#include "engine/config/config.hpp"
#include "engine/events/event_service.hpp"
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

    // TODO: remove this later when we have a proper gameplay screen
    state.player_pos.x = static_cast<float>(config.render.target_width) * 0.5F;
    state.player_pos.y = static_cast<float>(config.render.target_height) * 0.5F;

    engine::render::RenderQueue render_queue{};
    game::render::SceneRenderer scene_renderer{};
    engine::ui::UiSystem ui_system{platform, renderer, resources, config.render};
    engine::events::EventService event_service{};
    game::state::ChatState initial_chat_state{};
    game::state::ChatStore chat_store{std::move(initial_chat_state), game::state::reduce_chat_state};

    auto backend = std::make_unique<engine::backend::telegram::TelegramBackend>(
        event_service,
        "telegram"
    );
    engine::backend::NetworkManager network_manager{std::move(backend)};
    const auto start_result = network_manager.start();
    if (!start_result.has_value()) {
        std::cerr << "Network manager failed: " << start_result.error() << std::endl;
        return;
    }

    state.backend_connecting = true;

    auto backend_subscriptions = engine::backend::register_event_handlers(
        event_service,
        chat_store,
        network_manager
    );

    game::ui::initialize(ui_system, state, network_manager, chat_store);

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

        event_service.dispatch();
        pipeline.run(ctx);
    }

    network_manager.stop();
}

}  // namespace game


