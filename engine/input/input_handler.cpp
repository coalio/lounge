#include "engine/input/input_handler.hpp"

#include "engine/input/input_state.hpp"

#include <SDL.h>
#include <cstdint>

namespace engine::input {

void InputHandler::update(InputState& state) noexcept {
    state.reset();

    SDL_Event e;
    while (SDL_PollEvent(&e) == 1) {
        if (e.type == SDL_QUIT) {
            state.quit = true;
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            state.quit = true;
        }
    }

    const std::uint8_t* keys = SDL_GetKeyboardState(nullptr);
    state.up = keys[SDL_SCANCODE_W] != 0;
    state.down = keys[SDL_SCANCODE_S] != 0;
    state.left = keys[SDL_SCANCODE_A] != 0;
    state.right = keys[SDL_SCANCODE_D] != 0;
}

}  // namespace engine::input


