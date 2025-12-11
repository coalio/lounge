#pragma once

#include <expected>
#include <string>
#include <string_view>

namespace engine::config {

struct RenderSettings {
    int target_width{0};
    int target_height{0};
};

struct GameSettings {
    RenderSettings render{};
};

inline constexpr RenderSettings DEFAULT_RENDER_SETTINGS{
    .target_width = 1920,
    .target_height = 1080
};

inline constexpr GameSettings DEFAULT_GAME_SETTINGS{
    .render = DEFAULT_RENDER_SETTINGS
};

class ConfigService {
public:
    ConfigService() = delete;

    static auto load(std::string_view path) -> std::expected<void, std::string>;
    static auto ref() -> const GameSettings&;
};

}  // namespace engine::config




