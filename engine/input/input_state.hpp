#pragma once

namespace engine::input {

struct InputState {
    bool up{false};
    bool down{false};
    bool left{false};
    bool right{false};
    bool quit{false};

    inline void reset() noexcept {
        up = false;
        down = false;
        left = false;
        right = false;
        quit = false;
    }
};

}  // namespace engine::input


