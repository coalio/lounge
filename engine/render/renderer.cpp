#include "engine/render/renderer.hpp"

#include "engine/core/types.hpp"
#include "engine/platform/sdl_platform.hpp"

#include <SDL.h>
#include <cmath>
#include <string>

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

auto Renderer::create(engine::platform::SdlPlatform& platform,
                      const engine::config::GameSettings& game_settings)
    -> std::expected<Renderer, std::string> {
    const auto render_settings = game_settings.render;

    SDL_Renderer* r = SDL_CreateRenderer(
        platform.native_window(),
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (r == nullptr) {
        return std::unexpected(std::string{"SDL_CreateRenderer failed: "} + SDL_GetError());
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    if (SDL_RenderSetLogicalSize(r, render_settings.target_width, render_settings.target_height) != 0) {
        SDL_DestroyRenderer(r);
        return std::unexpected(std::string{"SDL_RenderSetLogicalSize failed: "} + SDL_GetError());
    }

    SDL_RenderSetIntegerScale(r, SDL_FALSE);

    return Renderer{r, render_settings};
}

Renderer::Renderer(SDL_Renderer* renderer, engine::config::RenderSettings render_settings) noexcept
    : renderer_{renderer},
      render_settings_{render_settings} {}

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


