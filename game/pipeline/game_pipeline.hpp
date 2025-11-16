#pragma once

#include "engine/pipeline/pipeline.hpp"
#include "engine/render/render_queue.hpp"

namespace engine::input {
class InputHandler;
struct InputState;
}

namespace engine::render {
class Renderer;
}

namespace engine::platform {
class SdlPlatform;
}

namespace game {
struct GameState;
}

namespace game::render {
class SceneRenderer;
}

namespace game::pipeline {

struct GameContext {
    engine::platform::SdlPlatform& platform;
    engine::input::InputHandler& input_handler;
    engine::input::InputState& input_state;
    game::GameState& game_state;
    engine::render::RenderQueue& render_queue;
    game::render::SceneRenderer& scene_renderer;
    float dt{0.0F};
    bool running{true};
};

class GamePipeline {
public:
    GamePipeline();
    GamePipeline(const GamePipeline&) = delete;
    auto operator=(const GamePipeline&) -> GamePipeline& = delete;
    GamePipeline(GamePipeline&&) = delete;
    auto operator=(GamePipeline&&) -> GamePipeline& = delete;
    ~GamePipeline() = default;

    void run(GameContext& ctx) const;

private:
    engine::pipeline::Pipeline<GameContext> pipeline_{};
};

}  // namespace game::pipeline


