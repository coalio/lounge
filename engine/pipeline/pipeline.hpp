#pragma once

#include <functional>
#include <utility>
#include <vector>

namespace engine::pipeline {

template <typename Ctx>
using Stage = std::function<void(Ctx&)>;

template <typename Ctx>
class Pipeline {
public:
    void add_stage(Stage<Ctx> stage) {
        stages_.push_back(std::move(stage));
    }

    void run(Ctx& ctx) const {
        for (const auto& stage : stages_) {
            stage(ctx);
        }
    }

private:
    std::vector<Stage<Ctx>> stages_{};
};

}  // namespace engine::pipeline


