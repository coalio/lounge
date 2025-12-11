#pragma once

#include <memory>
#include <string_view>

#include "engine/config/config.hpp"
#include "engine/resources/resource_manager.hpp"
#include "engine/ui/ui_context.hpp"

namespace engine::platform {
class SdlPlatform;
}

namespace engine::render {
class Renderer;
}

namespace engine::ui {

class UiSystem {
public:
    UiSystem(engine::platform::SdlPlatform& platform,
             engine::render::Renderer& renderer,
             engine::resources::ResourceManager& resources,
             const engine::config::RenderSettings& render_settings);
    UiSystem(const UiSystem&) = delete;
    auto operator=(const UiSystem&) -> UiSystem& = delete;
    UiSystem(UiSystem&&) = delete;
    auto operator=(UiSystem&&) -> UiSystem& = delete;
    ~UiSystem();

    [[nodiscard]] auto context() noexcept -> UiContext&;
    [[nodiscard]] auto context() const noexcept -> const UiContext&;

    void update(float dt);
    void render();
    void process_event(const SDL_Event& event);
    void load_font(std::string_view path);

private:
    std::unique_ptr<UiBackend> backend_{};
    UiContext context_;
};

}  // namespace engine::ui

