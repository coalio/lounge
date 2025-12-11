#include "engine/ui/ui_system.hpp"

#include <utility>

#include "engine/render/renderer.hpp"
#include "engine/ui/backends/rml/rml_ui_backend.hpp"

namespace engine::ui {

namespace {

auto make_backend(engine::platform::SdlPlatform& platform,
                  engine::render::Renderer& renderer,
                  engine::resources::ResourceManager& resources,
                  const engine::config::RenderSettings& render_settings)
    -> std::unique_ptr<UiBackend> {
    return std::make_unique<engine::ui::backends::rml::RmlUiBackend>(
        platform,
        renderer,
        resources,
        render_settings
    );
}

}  // namespace

UiSystem::UiSystem(engine::platform::SdlPlatform& platform,
                   engine::render::Renderer& renderer,
                   engine::resources::ResourceManager& resources,
                   const engine::config::RenderSettings& render_settings)
    : backend_{make_backend(platform, renderer, resources, render_settings)},
      context_{*backend_, resources} {
    backend_->initialize();
}

UiSystem::~UiSystem() {
    backend_->shutdown();
}

auto UiSystem::context() noexcept -> UiContext& {
    return context_;
}

auto UiSystem::context() const noexcept -> const UiContext& {
    return context_;
}

void UiSystem::update(float dt) {
    context_.update(dt);
}

void UiSystem::render() {
    context_.render();
}

void UiSystem::process_event(const SDL_Event& event) {
    context_.process_event(event);
}

void UiSystem::load_font(std::string_view path) {
    backend_->load_font(path);
}

}  // namespace engine::ui

