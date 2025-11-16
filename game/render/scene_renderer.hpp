#pragma once

#include "engine/render/render_queue.hpp"
#include "game/state.hpp"

namespace game::render {

class SceneRenderer {
public:
    SceneRenderer() = default;
    SceneRenderer(const SceneRenderer&) = delete;
    auto operator=(const SceneRenderer&) -> SceneRenderer& = delete;
    SceneRenderer(SceneRenderer&&) = delete;
    auto operator=(SceneRenderer&&) -> SceneRenderer& = delete;
    ~SceneRenderer() = default;

    void build_queue(const game::GameState& state, engine::render::RenderQueue& queue) const;
};

}  // namespace game::render


