#include "game/render/scene_renderer.hpp"

#include "engine/core/types.hpp"

namespace game::render {

// Here we'll get the context of visible actors from a previous stage of the pipeline, so we only
// render what's necessary. Right now it does absolutely nothing but eventually it will
void SceneRenderer::build_queue(const game::GameState& state, engine::render::RenderQueue& queue) const {
    queue.clear();

    queue.push_back(engine::render::Clear{engine::core::COLOR_BLACK});

    // TODO: remove this once we have a proper gameplay system based on actors and hierarchy trees
    // this is just an example of how we should add things to the engine
    // if (!state.gameplay_active) {
    //     return;
    // }

    // const float half = state.player_size * 0.5F;
    // const engine::core::Rect rect{
    //     state.player_pos.x - half,
    //     state.player_pos.y - half,
    //     state.player_size,
    //     state.player_size
    // };

    // queue.push_back(engine::render::FillRect{rect, engine::core::COLOR_WHITE});
    
}

}  // namespace game::render


