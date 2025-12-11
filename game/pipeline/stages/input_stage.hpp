#pragma once

namespace game::pipeline {
struct GameContext;
}

namespace game::pipeline::stages {

class InputStage {
public:
    InputStage() = default;
    void run(GameContext& ctx);
};

}  // namespace game::pipeline::stages

