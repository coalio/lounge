#pragma once

namespace engine::input {

struct InputState;

class InputHandler {
public:
    InputHandler() = default;
    InputHandler(const InputHandler&) = delete;
    auto operator=(const InputHandler&) -> InputHandler& = delete;
    InputHandler(InputHandler&&) = delete;
    auto operator=(InputHandler&&) -> InputHandler& = delete;
    ~InputHandler() = default;

    void update(InputState& state) noexcept;
};

}  // namespace engine::input


