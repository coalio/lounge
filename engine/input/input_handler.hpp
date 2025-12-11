#pragma once

#include <vector>

#include <SDL.h>

namespace engine::input {

struct InputState;

class InputHandler {
public:
    InputHandler() = default;
    InputHandler(InputHandler&&) = delete;
    InputHandler(const InputHandler&) = delete;
    ~InputHandler() = default;

    auto operator=(const InputHandler&) -> InputHandler& = delete;
    auto operator=(InputHandler&&) -> InputHandler& = delete;

    void update(InputState& state) noexcept;
    [[nodiscard]] auto events() const noexcept -> const std::vector<SDL_Event>&;

private:
    std::vector<SDL_Event> events_{};
};

}  // namespace engine::input


