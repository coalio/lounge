#pragma once

#include <expected>
#include <string>
#include <string_view>

namespace engine::config {

struct RenderSettings {
    int target_width{0};
    int target_height{0};
};

struct TelegramSettings {
    int api_id{0};
    std::string api_hash{};
    bool save_credentials{true};
};

struct GameSettings {
    RenderSettings render{};
    TelegramSettings telegram{};
};

inline constexpr RenderSettings DEFAULT_RENDER_SETTINGS{
    .target_width = 1920,
    .target_height = 1080
};

inline constexpr GameSettings DEFAULT_GAME_SETTINGS{
    .render = DEFAULT_RENDER_SETTINGS,
    .telegram = TelegramSettings{}
};

class ConfigService {
public:
    ConfigService() = delete;

    static auto load(std::string_view path) -> std::expected<void, std::string>;
    static auto ref() -> const GameSettings&;
    static auto set_telegram_credentials(int api_id, std::string api_hash) -> std::expected<void, std::string>;
};

}  // namespace engine::config




