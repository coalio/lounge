#include <expected>
#include <iostream>

#include "engine/platform/sdl_platform.hpp"
#include "engine/render/renderer.hpp"
#include "engine/resources/resource_manager.hpp"

namespace game {
	auto run_game(engine::platform::SdlPlatform& platform,
	              engine::render::Renderer& renderer,
	              engine::resources::ResourceManager& resources) -> void;
}

auto main() -> int {
	constexpr int WINDOW_WIDTH = 800;
	constexpr int WINDOW_HEIGHT = 600;
	constexpr const char* WINDOW_TITLE = "Lounge";

	auto platform_expected =
	    engine::platform::SdlPlatform::create(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

	if (!platform_expected.has_value()) {
		std::cerr << "Platform init failed: " << platform_expected.error() << std::endl;
		return 1;
	}

	auto platform = std::move(platform_expected.value());

	auto renderer_expected = engine::render::Renderer::create(platform);
	if (!renderer_expected.has_value()) {
		std::cerr << "Renderer init failed: " << renderer_expected.error() << std::endl;
		return 1;
	}

	auto renderer = std::move(renderer_expected.value());

	engine::resources::ResourceManager resources{};

	game::run_game(platform, renderer, resources);

	return 0;
}
