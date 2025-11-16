#include "engine/platform/sdl_platform.hpp"

#include <SDL.h>
#include <string>
#include <utility>

namespace engine::platform {

auto SdlPlatform::create(int width, int height, std::string_view title)
    -> std::expected<SdlPlatform, std::string> {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return std::unexpected(std::string{"SDL_Init failed: "} + SDL_GetError());
    }

    SDL_Window* window = SDL_CreateWindow(
        title.data(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        const std::string err{"SDL_CreateWindow failed: "};
        SDL_Quit();
        return std::unexpected(err + SDL_GetError());
    }

    const auto freq = static_cast<std::uint64_t>(SDL_GetPerformanceFrequency());
    const auto counter = static_cast<std::uint64_t>(SDL_GetPerformanceCounter());

    return SdlPlatform{window, width, height, freq, counter};
}

SdlPlatform::SdlPlatform(SDL_Window* window,
                         int width,
                         int height,
                         std::uint64_t perf_freq,
                         std::uint64_t last_counter) noexcept
    : window_{window},
      width_{width},
      height_{height},
      perf_freq_{perf_freq},
      last_counter_{last_counter} {}

SdlPlatform::SdlPlatform(SdlPlatform&& other) noexcept
    : window_{other.window_},
      width_{other.width_},
      height_{other.height_},
      perf_freq_{other.perf_freq_},
      last_counter_{other.last_counter_} {
    other.window_ = nullptr;
    other.width_ = 0;
    other.height_ = 0;
    other.perf_freq_ = 0;
    other.last_counter_ = 0;
}

auto SdlPlatform::operator=(SdlPlatform&& other) noexcept -> SdlPlatform& {
    if (this != &other) {
        if (window_ != nullptr) {
            SDL_DestroyWindow(window_);
            SDL_Quit();
        }
        window_ = other.window_;
        width_ = other.width_;
        height_ = other.height_;
        perf_freq_ = other.perf_freq_;
        last_counter_ = other.last_counter_;

        other.window_ = nullptr;
        other.width_ = 0;
        other.height_ = 0;
        other.perf_freq_ = 0;
        other.last_counter_ = 0;
    }
    return *this;
}

SdlPlatform::~SdlPlatform() {
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
    }
}

auto SdlPlatform::width() const noexcept -> int {
    return width_;
}

auto SdlPlatform::height() const noexcept -> int {
    return height_;
}

auto SdlPlatform::compute_delta_seconds() -> float {
    const auto current = static_cast<std::uint64_t>(SDL_GetPerformanceCounter());
    const auto delta_count = current - last_counter_;
    last_counter_ = current;

    if (perf_freq_ == 0U) {
        return 0.0F;
    }

    const double seconds = static_cast<double>(delta_count) / static_cast<double>(perf_freq_);
    return static_cast<float>(seconds);
}

auto SdlPlatform::native_window() const noexcept -> SDL_Window* {
    return window_;
}

}  // namespace engine::platform


