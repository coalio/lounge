#include <expected>
#include <filesystem>
#include <iostream>
#include <system_error>

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

	std::error_code work_dir_error{};
	const auto work_dir = std::filesystem::current_path(work_dir_error);
	if (work_dir_error) {
		std::cerr << "Failed to determine working directory: " << work_dir_error.message()
		          << std::endl;
		return 1;
	}

	const auto config_path = work_dir / "config/game.toml";
	const auto config_result = engine::config::ConfigService::load(config_path.string());
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

	engine::resources::ResourceManager resources{work_dir};
	game::run_game(platform, renderer, resources);

	return 0;
}
