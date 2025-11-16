#include "engine/render/renderer.hpp"

#include "engine/core/types.hpp"
#include "engine/platform/sdl_platform.hpp"

#include <SDL.h>
#include <cmath>
#include <utility>

namespace engine::render {

namespace {

inline void set_draw_color(SDL_Renderer* r, const core::Color color) noexcept {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
}

inline auto to_sdl_rect(const core::Rect rect) noexcept -> SDL_Rect {
    const int x = static_cast<int>(std::lround(rect.x));
    const int y = static_cast<int>(std::lround(rect.y));
    const int w = static_cast<int>(std::lround(rect.w));
    const int h = static_cast<int>(std::lround(rect.h));
    return SDL_Rect{x, y, w, h};
}

}  // namespace

auto Renderer::create(engine::platform::SdlPlatform& platform)
    -> std::expected<Renderer, std::string> {
    SDL_Renderer* r = SDL_CreateRenderer(
        platform.native_window(),
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (r == nullptr) {
        return std::unexpected(std::string{"SDL_CreateRenderer failed: "} + SDL_GetError());
    }

    return Renderer{r};
}

Renderer::Renderer(SDL_Renderer* renderer) noexcept : renderer_{renderer} {}

Renderer::Renderer(Renderer&& other) noexcept : renderer_{other.renderer_} {
    other.renderer_ = nullptr;
}

auto Renderer::operator=(Renderer&& other) noexcept -> Renderer& {
    if (this != &other) {
        if (renderer_ != nullptr) {
            SDL_DestroyRenderer(renderer_);
        }
        renderer_ = other.renderer_;
        other.renderer_ = nullptr;
    }
    return *this;
}

Renderer::~Renderer() {
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
}

void Renderer::begin_frame() noexcept {
    // no-op; Clear is driven by the queue
}

void Renderer::flush(const RenderQueue& queue) noexcept {
    for (const auto& cmd : queue) {
        std::visit(
            [this](const auto& c) {
                using T = std::decay_t<decltype(c)>;
                if constexpr (std::is_same_v<T, Clear>) {
                    set_draw_color(renderer_, c.color);
                    SDL_RenderClear(renderer_);
                } else if constexpr (std::is_same_v<T, FillRect>) {
                    set_draw_color(renderer_, c.color);
                    const SDL_Rect rect = to_sdl_rect(c.rect);
                    SDL_RenderFillRect(renderer_, &rect);
                }
            },
            cmd
        );
    }
}

void Renderer::end_frame() noexcept {
    SDL_RenderPresent(renderer_);
}

}  // namespace engine::render


