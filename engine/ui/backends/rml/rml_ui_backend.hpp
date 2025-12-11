#pragma once

#include <RmlUi/Core.h>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "engine/config/config.hpp"
#include "engine/ui/ui_backend.hpp"

namespace engine::platform {
class SdlPlatform;
}

namespace engine::render {
class Renderer;
}

namespace engine::resources {
class ResourceManager;
}

namespace engine::ui::backends::rml {

class RmlRenderInterface;
class RmlSystemInterface;

class RmlUiBackend : public UiBackend {
public:
    RmlUiBackend(engine::platform::SdlPlatform& platform,
                 engine::render::Renderer& renderer,
                 engine::resources::ResourceManager& resources,
                 const engine::config::RenderSettings& render_settings);
    RmlUiBackend(const RmlUiBackend&) = delete;
    auto operator=(const RmlUiBackend&) -> RmlUiBackend& = delete;
    RmlUiBackend(RmlUiBackend&&) = delete;
    auto operator=(RmlUiBackend&&) -> RmlUiBackend& = delete;
    ~RmlUiBackend() override;

    void initialize() override;
    void shutdown() override;
    void update(float dt) override;
    void render() override;
    void process_event(const SDL_Event& event) override;
    void sync_documents(const std::vector<UiDocument>& documents) override;
    void load_font(std::string_view path) override;

private:
    struct ListenerHolder;
    struct ListenerBinding {
        Rml::Element* element{nullptr};
        std::string event{};
        std::unique_ptr<ListenerHolder> listener{};
    };
    struct DocumentRecord {
        Rml::ElementDocument* document{nullptr};
        std::vector<ListenerBinding> listeners{};
    };

    void destroy_documents();
    void attach_listeners(Rml::ElementDocument& document,
                          const UiDocument& definition,
                          std::vector<ListenerBinding>& out_listeners);
    void detach_listeners(std::vector<ListenerBinding>& listeners);
    static auto translate_key(SDL_Keycode key) -> Rml::Input::KeyIdentifier;
    static auto translate_modifiers(SDL_Keymod mods) -> int;

    engine::platform::SdlPlatform& platform_;
    engine::render::Renderer& renderer_;
    engine::resources::ResourceManager& resources_;
    engine::config::RenderSettings render_settings_{};

    std::unique_ptr<RmlSystemInterface> system_interface_{};
    std::unique_ptr<RmlRenderInterface> render_interface_{};
    Rml::Context* context_{nullptr};
    std::vector<DocumentRecord> documents_{};
};

}  // namespace engine::ui::backends::rml

