#pragma once

#include "engine/config/config.hpp"
#include "engine/render/render_queue.hpp"

#include <SDL.h>
#include <expected>
#include <string>

struct SDL_Renderer;

namespace engine::platform {
class SdlPlatform;
}

namespace engine::render {

class Renderer {
public:
    static auto create(engine::platform::SdlPlatform& platform,
                       const engine::config::GameSettings& game_settings)
        -> std::expected<Renderer, std::string>;

    Renderer() = delete;
    Renderer(const Renderer&) = delete;
    auto operator=(const Renderer&) -> Renderer& = delete;
    Renderer(Renderer&& other) noexcept;
    auto operator=(Renderer&& other) noexcept -> Renderer&;
    ~Renderer();

    void begin_frame() noexcept;
    void flush(const RenderQueue& queue) noexcept;
    void end_frame() noexcept;
    [[nodiscard]] auto native_handle() const noexcept -> SDL_Renderer*;

private:
    Renderer(SDL_Renderer* renderer, engine::config::RenderSettings render_settings) noexcept;

    SDL_Renderer* renderer_{nullptr};
    engine::config::RenderSettings render_settings_{};
};

inline Renderer::Renderer(Renderer&& other) noexcept
    : renderer_{other.renderer_},
      render_settings_{other.render_settings_} {
    other.renderer_ = nullptr;
}

inline auto Renderer::operator=(Renderer&& other) noexcept -> Renderer& {
    if (this != &other) {
        if (renderer_ != nullptr) {
            SDL_DestroyRenderer(renderer_);
        }
        renderer_ = other.renderer_;
        render_settings_ = other.render_settings_;
        other.renderer_ = nullptr;
    }
    return *this;
}

inline Renderer::~Renderer() {
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
}

inline auto Renderer::native_handle() const noexcept -> SDL_Renderer* {
    return renderer_;
}

}  // namespace engine::render


