#pragma once

#include <cstdint>

namespace engine::core {

struct Vec2 {
    float x{0.0F};
    float y{0.0F};
};

struct Color {
    std::uint8_t r{0U};
    std::uint8_t g{0U};
    std::uint8_t b{0U};
    std::uint8_t a{255U};
};

struct Rect {
    float x{0.0F};
    float y{0.0F};
    float w{0.0F};
    float h{0.0F};
};

inline constexpr Color COLOR_BLACK{0U, 0U, 0U, 255U};
inline constexpr Color COLOR_WHITE{255U, 255U, 255U, 255U};

}  // namespace engine::core


