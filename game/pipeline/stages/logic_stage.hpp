#pragma once

namespace game::pipeline {
struct GameContext;
}

namespace game::pipeline::stages {

class LogicStage {
public:
    LogicStage() = default;
    void run(GameContext& ctx);
};

}  // namespace game::pipeline::stages

