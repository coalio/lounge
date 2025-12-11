#include <expected>
#include <filesystem>
#include <iostream>

#include "engine/config/config.hpp"
#include "engine/platform/sdl_platform.hpp"
#include "engine/render/renderer.hpp"
#include "engine/resources/resource_manager.hpp"

namespace game {
	auto run_game(engine::platform::SdlPlatform& platform,
	              engine::render::Renderer& renderer,
	              engine::resources::ResourceManager& resources) -> void;
}

auto main() -> int {
	constexpr const char* WINDOW_TITLE = "Lounge";
	constexpr const char* CONFIG_PATH = "config/game.toml";

	const auto config_result = engine::config::ConfigService::load(CONFIG_PATH);
	if (!config_result.has_value()) {
		std::cerr << "Config load failed: " << config_result.error() << std::endl;
		return 1;
	}

	const auto& config = engine::config::ConfigService::ref();

	auto platform_expected = engine::platform::SdlPlatform::create(
	    config.render.target_width,
	    config.render.target_height,
	    WINDOW_TITLE
	);

	if (!platform_expected.has_value()) {
		std::cerr << "Platform init failed: " << platform_expected.error() << std::endl;
		return 1;
	}

	auto platform = std::move(platform_expected.value());

	auto renderer_expected = engine::render::Renderer::create(platform, config);
	if (!renderer_expected.has_value()) {
		std::cerr << "Renderer init failed: " << renderer_expected.error() << std::endl;
		return 1;
	}

	auto renderer = std::move(renderer_expected.value());

	const std::filesystem::path asset_root = std::filesystem::path{LOUNGE_ASSET_ROOT};
	engine::resources::ResourceManager resources{asset_root};

	game::run_game(platform, renderer, resources);

	return 0;
}
