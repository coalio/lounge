#include "engine/config/config.hpp"

#include "toml.hpp"

#include <filesystem>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>

namespace engine::config {
namespace {

struct ConfigStore {
    GameSettings config{};
};

auto store() -> ConfigStore& {
    static ConfigStore instance{};
    return instance;
}

auto write_default_config(const std::filesystem::path& config_path)
    -> std::expected<void, std::string> {
    if (config_path.has_parent_path()) {
        std::error_code err{};
        std::filesystem::create_directories(config_path.parent_path(), err);
        if (err) {
            std::ostringstream oss;
            oss << "Failed to create config directory '" << config_path.parent_path().string()
                << "': " << err.message();
            return std::unexpected(oss.str());
        }
    }

    std::ofstream file{config_path};
    if (!file) {
        std::ostringstream oss;
        oss << "Failed to create default config file at '" << config_path.string() << "'.";
        return std::unexpected(oss.str());
    }

    const auto& defaults = DEFAULT_GAME_SETTINGS;

    if (defaults.render.target_width != 0 || defaults.render.target_height != 0) {
        file << "[render]\n";
        file << "target_width = " << defaults.render.target_width << "\n";
        file << "target_height = " << defaults.render.target_height << "\n";
        file << "\n";
    }

    if (!file.good()) {
        std::ostringstream oss;
        oss << "Failed to write default config file at '" << config_path.string() << "'.";
        return std::unexpected(oss.str());
    }

    return {};
}

inline auto is_sixteen_nine(int width, int height) noexcept -> bool {
    return width * 9 == height * 16;
}

inline auto narrow_int(std::string_view key, int64_t value)
    -> std::expected<int, std::string> {
    if (value <= 0 || value > static_cast<int64_t>(std::numeric_limits<int>::max())) {
        std::ostringstream oss;
        oss << "Config value '" << key << "' must be positive and fit in a 32-bit int.";
        return std::unexpected(oss.str());
    }

    return static_cast<int>(value);
}

inline auto parse_render_settings(const toml::table& table,
                                  RenderSettings defaults)
    -> std::expected<RenderSettings, std::string> {
    auto result = defaults;

    if (const auto width_node = table.get("target_width")) {
        if (const auto width_value = width_node->value<int64_t>()) {
            auto width_expected = narrow_int("render.target_width", *width_value);
            if (!width_expected.has_value()) {
                return std::unexpected(width_expected.error());
            }

            result.target_width = width_expected.value();
        }
    }

    if (const auto height_node = table.get("target_height")) {
        if (const auto height_value = height_node->value<int64_t>()) {
            auto height_expected = narrow_int("render.target_height", *height_value);
            if (!height_expected.has_value()) {
                return std::unexpected(height_expected.error());
            }

            result.target_height = height_expected.value();
        }
    }

    if (!is_sixteen_nine(result.target_width, result.target_height)) {
        std::ostringstream oss;
        oss << "Render resolution " << result.target_width << "x" << result.target_height
            << " does not maintain a 16:9 aspect ratio.";
        return std::unexpected(oss.str());
    }

    return result;
}

}  // namespace

auto ConfigService::load(std::string_view path) -> std::expected<void, std::string> {
    const std::filesystem::path config_path{path};

    if (!std::filesystem::exists(config_path)) {
        auto create_result = write_default_config(config_path);
        if (!create_result.has_value()) {
            return create_result;
        }
    }

    try {
        auto table = toml::parse_file(config_path.string());
        auto config = DEFAULT_GAME_SETTINGS;

        if (const auto render_table = table["render"].as_table()) {
            auto render_expected = parse_render_settings(*render_table, config.render);
            if (!render_expected.has_value()) {
                return std::unexpected(render_expected.error());
            }

            config.render = render_expected.value();
        }

        auto& cfg_store = store();
        cfg_store.config = config;

        return {};
    } catch (const toml::parse_error& err) {
        std::ostringstream oss;
        oss << "Failed to parse config: " << err.description();
        return std::unexpected(oss.str());
    }
}

auto ConfigService::ref() -> const GameSettings& {
    return store().config;
}

}  // namespace engine::config



