#pragma once

#include "engine/core/types.hpp"

#include <variant>
#include <vector>

namespace engine::render {

struct Clear {
    engine::core::Color color{};
};

struct FillRect {
    engine::core::Rect rect{};
    engine::core::Color color{};
};

using RenderCommand = std::variant<Clear, FillRect>;
using RenderQueue = std::vector<RenderCommand>;

}  // namespace engine::render


