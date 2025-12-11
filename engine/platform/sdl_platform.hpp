#pragma once

#include <SDL.h>
#include <cstdint>
#include <expected>
#include <string_view>

struct SDL_Window;

namespace engine::platform {

class SdlPlatform {
public:
    static auto create(int width, int height, std::string_view title)
        -> std::expected<SdlPlatform, std::string>;

    SdlPlatform() = delete;
    SdlPlatform(const SdlPlatform&) = delete;
    auto operator=(const SdlPlatform&) -> SdlPlatform& = delete;
    SdlPlatform(SdlPlatform&& other) noexcept;
    auto operator=(SdlPlatform&& other) noexcept -> SdlPlatform&;
    ~SdlPlatform();

    auto width() const noexcept -> int;
    auto height() const noexcept -> int;

    auto compute_delta_seconds() -> float;

    auto native_window() const noexcept -> SDL_Window*;

private:
    SdlPlatform(SDL_Window* window, int width, int height,
                std::uint64_t perf_freq, std::uint64_t last_counter) noexcept;

    SDL_Window* window_{nullptr};
    int width_{0};
    int height_{0};
    std::uint64_t perf_freq_{0};
    std::uint64_t last_counter_{0};
};

inline SdlPlatform::SdlPlatform(SdlPlatform&& other) noexcept
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

inline auto SdlPlatform::operator=(SdlPlatform&& other) noexcept -> SdlPlatform& {
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

inline SdlPlatform::~SdlPlatform() {
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
    }
}

}  // namespace engine::platform


