#pragma once

#include <SDL.h>
#include <string_view>
#include <vector>

#include "engine/ui/ui_document.hpp"

namespace engine::ui {

class UiBackend {
public:
    UiBackend() = default;
    UiBackend(const UiBackend&) = delete;
    auto operator=(const UiBackend&) -> UiBackend& = delete;
    UiBackend(UiBackend&&) = delete;
    auto operator=(UiBackend&&) -> UiBackend& = delete;
    virtual ~UiBackend() = default;

    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual void process_event(const SDL_Event& event) = 0;
    virtual void sync_documents(const std::vector<UiDocument>& documents) = 0;
    virtual void load_font(std::string_view path) = 0;
};

}  // namespace engine::ui

