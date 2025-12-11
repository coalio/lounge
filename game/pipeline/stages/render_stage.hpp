#pragma once

namespace game::pipeline {
struct GameContext;
}

namespace game::pipeline::stages {

class RenderStage {
public:
    RenderStage() = default;
    void run(GameContext& ctx);
};

}  // namespace game::pipeline::stages

