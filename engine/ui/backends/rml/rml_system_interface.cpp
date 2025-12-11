#include "engine/ui/backends/rml/rml_system_interface.hpp"

#include <SDL.h>

namespace engine::ui::backends::rml {

RmlSystemInterface::RmlSystemInterface() = default;

auto RmlSystemInterface::GetElapsedTime() -> double {
    const auto now = std::chrono::steady_clock::now();
    const auto delta = std::chrono::duration<double>(now - start_);
    return delta.count();
}

void RmlSystemInterface::SetClipboardText(const Rml::String& text) {
    clipboard_ = text;
    SDL_SetClipboardText(clipboard_.c_str());
}

void RmlSystemInterface::GetClipboardText(Rml::String& text) {
    if (const char* stored = SDL_GetClipboardText()) {
        text = stored;
        SDL_free(const_cast<char*>(stored));
    } else {
        text = clipboard_;
    }
}

bool RmlSystemInterface::LogMessage(Rml::Log::Type type, const Rml::String& message) {
    switch (type) {
        case Rml::Log::LT_ERROR:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "RmlUi: %s", message.c_str());
            break;
        case Rml::Log::LT_WARNING:
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "RmlUi: %s", message.c_str());
            break;
        default:
            SDL_Log("RmlUi: %s", message.c_str());
            break;
    }
    return true;
}

}  // namespace engine::ui::backends::rml

