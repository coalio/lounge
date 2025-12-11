#pragma once

#include <RmlUi/Core/SystemInterface.h>

#include <chrono>

namespace engine::ui::backends::rml {

class RmlSystemInterface : public Rml::SystemInterface {
public:
    RmlSystemInterface();
    ~RmlSystemInterface() override = default;

    auto GetElapsedTime() -> double override;
    void SetClipboardText(const Rml::String& text) override;
    void GetClipboardText(Rml::String& text) override;
    bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;

private:
    std::chrono::steady_clock::time_point start_{std::chrono::steady_clock::now()};
    Rml::String clipboard_{};
};

}  // namespace engine::ui::backends::rml

