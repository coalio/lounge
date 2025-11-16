#pragma once

#include "engine/render/render_queue.hpp"

#include <expected>
#include <string>

struct SDL_Renderer;

namespace engine::platform {
class SdlPlatform;
}

namespace engine::render {

class Renderer {
public:
    static auto create(engine::platform::SdlPlatform& platform)
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

private:
    explicit Renderer(SDL_Renderer* renderer) noexcept;

    SDL_Renderer* renderer_{nullptr};
};

}  // namespace engine::render


