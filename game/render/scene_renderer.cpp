#include "game/render/scene_renderer.hpp"

#include "engine/core/types.hpp"

namespace game::render {

void SceneRenderer::build_queue(const game::GameState& state, engine::render::RenderQueue& queue) const {
    queue.clear();

    queue.push_back(engine::render::Clear{engine::core::COLOR_BLACK});

    const float half = state.player_size * 0.5F;
    const engine::core::Rect rect{
        state.player_pos.x - half,
        state.player_pos.y - half,
        state.player_size,
        state.player_size
    };

    queue.push_back(engine::render::FillRect{rect, engine::core::COLOR_WHITE});
}

}  // namespace game::render


