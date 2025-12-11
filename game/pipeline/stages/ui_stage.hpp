#pragma once

namespace game::pipeline {
struct GameContext;
}

namespace game::pipeline::stages {

class UiStage {
public:
    UiStage() = default;
    void run(GameContext& ctx);
};

}  // namespace game::pipeline::stages

