#include "engine/ui/backends/rml/rml_ui_backend.hpp"

#include <SDL.h>
#include <RmlUi/Core/Input.h>

#include <memory>

#include "engine/platform/sdl_platform.hpp"
#include "engine/render/renderer.hpp"
#include "engine/resources/resource_manager.hpp"
#include "engine/ui/backends/rml/rml_render_interface.hpp"
#include "engine/ui/backends/rml/rml_system_interface.hpp"

namespace engine::ui::backends::rml {

namespace {

auto to_mouse_button(Uint8 sdl_button) -> int {
    switch (sdl_button) {
        case SDL_BUTTON_LEFT:
            return 0;
        case SDL_BUTTON_RIGHT:
            return 1;
        case SDL_BUTTON_MIDDLE:
            return 2;
        default:
            return 0;
    }
}

}  // namespace

struct RmlUiBackend::ListenerHolder : public Rml::EventListener {
    explicit ListenerHolder(UiEventHandler handler)
        : handler_{std::move(handler)} {}

    void ProcessEvent(Rml::Event&) override {
        if (handler_) {
            handler_();
        }
    }

    void OnDetach(Rml::Element*) override {}

    UiEventHandler handler_{};
};

RmlUiBackend::RmlUiBackend(engine::platform::SdlPlatform& platform,
                           engine::render::Renderer& renderer,
                           engine::resources::ResourceManager& resources,
                           const engine::config::RenderSettings& render_settings)
    : platform_{platform},
      renderer_{renderer},
      resources_{resources},
      render_settings_{render_settings},
      system_interface_{std::make_unique<RmlSystemInterface>()},
      render_interface_{std::make_unique<RmlRenderInterface>(renderer.native_handle())} {}

RmlUiBackend::~RmlUiBackend() = default;

void RmlUiBackend::initialize() {
    Rml::SetSystemInterface(system_interface_.get());
    Rml::SetRenderInterface(render_interface_.get());
    Rml::Initialise();

    context_ = Rml::CreateContext(
        "lounge_ui",
        {render_settings_.target_width, render_settings_.target_height}
    );
}

void RmlUiBackend::shutdown() {
    destroy_documents();

    if (context_ != nullptr) {
        Rml::RemoveContext(context_->GetName());
        context_ = nullptr;
    }

    Rml::Shutdown();
}

void RmlUiBackend::update(float) {
    if (context_ != nullptr) {
        context_->Update();
    }
}

void RmlUiBackend::render() {
    if (context_ != nullptr) {
        context_->Render();
    }
}

void RmlUiBackend::process_event(const SDL_Event& event) {
    if (context_ == nullptr) {
        return;
    }

    switch (event.type) {
        case SDL_MOUSEMOTION:
            context_->ProcessMouseMove(event.motion.x, event.motion.y, 0);
            break;
        case SDL_MOUSEBUTTONDOWN:
            context_->ProcessMouseButtonDown(to_mouse_button(event.button.button), 0);
            break;
        case SDL_MOUSEBUTTONUP:
            context_->ProcessMouseButtonUp(to_mouse_button(event.button.button), 0);
            break;
        case SDL_MOUSEWHEEL:
            context_->ProcessMouseWheel(-event.wheel.y, 0);
            break;
        case SDL_TEXTINPUT:
            context_->ProcessTextInput(event.text.text);
            break;
        case SDL_KEYDOWN:
            context_->ProcessKeyDown(
                translate_key(event.key.keysym.sym),
                translate_modifiers(static_cast<SDL_Keymod>(event.key.keysym.mod))
            );
            break;
        case SDL_KEYUP:
            context_->ProcessKeyUp(
                translate_key(event.key.keysym.sym),
                translate_modifiers(static_cast<SDL_Keymod>(event.key.keysym.mod))
            );
            break;
        default:
            break;
    }
}

void RmlUiBackend::sync_documents(const std::vector<UiDocument>& documents) {
    if (context_ == nullptr) {
        return;
    }

    destroy_documents();
    documents_.reserve(documents.size());

    for (const auto& doc : documents) {
        DocumentRecord record{};
        record.document = context_->LoadDocumentFromMemory(doc.markup);
        if (record.document == nullptr) {
            continue;
        }

        record.document->Show();
        attach_listeners(*record.document, doc, record.listeners);
        documents_.push_back(std::move(record));
    }
}

void RmlUiBackend::load_font(std::string_view path) {
    const auto resolved = resources_.resolve(path);
    Rml::LoadFontFace(resolved.string().c_str());
}

void RmlUiBackend::destroy_documents() {
    for (auto& record : documents_) {
        detach_listeners(record.listeners);
        if (record.document != nullptr) {
            record.document->Close();
            record.document = nullptr;
        }
    }
    documents_.clear();
}

void RmlUiBackend::attach_listeners(Rml::ElementDocument& document,
                                    const UiDocument& definition,
                                    std::vector<ListenerBinding>& out_listeners) {
    out_listeners.clear();
    out_listeners.reserve(definition.events.size());

    for (const auto& binding : definition.events) {
        if (binding.id.empty() || binding.type.empty() || binding.handler == nullptr) {
            continue;
        }

        if (auto* element = document.GetElementById(binding.id.c_str())) {
            ListenerBinding listener_binding{};
            listener_binding.element = element;
            listener_binding.event = binding.type;
            listener_binding.listener = std::make_unique<ListenerHolder>(binding.handler);

            element->AddEventListener(listener_binding.event.c_str(), listener_binding.listener.get());
            out_listeners.push_back(std::move(listener_binding));
        }
    }
}

void RmlUiBackend::detach_listeners(std::vector<ListenerBinding>& listeners) {
    for (auto& binding : listeners) {
        if (binding.element != nullptr && binding.listener != nullptr && !binding.event.empty()) {
            binding.element->RemoveEventListener(binding.event.c_str(), binding.listener.get());
        }
    }
    listeners.clear();
}

auto RmlUiBackend::translate_key(SDL_Keycode key) -> Rml::Input::KeyIdentifier {
    using Rml::Input::KI_0;
    using Rml::Input::KI_1;
    using Rml::Input::KI_2;
    using Rml::Input::KI_3;
    using Rml::Input::KI_4;
    using Rml::Input::KI_5;
    using Rml::Input::KI_6;
    using Rml::Input::KI_7;
    using Rml::Input::KI_8;
    using Rml::Input::KI_9;
    using Rml::Input::KI_A;
    using Rml::Input::KI_B;
    using Rml::Input::KI_BACK;
    using Rml::Input::KI_C;
    using Rml::Input::KI_D;
    using Rml::Input::KI_DELETE;
    using Rml::Input::KI_DOWN;
    using Rml::Input::KI_E;
    using Rml::Input::KI_ESCAPE;
    using Rml::Input::KI_F;
    using Rml::Input::KI_G;
    using Rml::Input::KI_H;
    using Rml::Input::KI_I;
    using Rml::Input::KI_J;
    using Rml::Input::KI_K;
    using Rml::Input::KI_L;
    using Rml::Input::KI_LEFT;
    using Rml::Input::KI_M;
    using Rml::Input::KI_N;
    using Rml::Input::KI_O;
    using Rml::Input::KI_P;
    using Rml::Input::KI_Q;
    using Rml::Input::KI_R;
    using Rml::Input::KI_RETURN;
    using Rml::Input::KI_RIGHT;
    using Rml::Input::KI_S;
    using Rml::Input::KI_SPACE;
    using Rml::Input::KI_T;
    using Rml::Input::KI_TAB;
    using Rml::Input::KI_U;
    using Rml::Input::KI_UNKNOWN;
    using Rml::Input::KI_V;
    using Rml::Input::KI_W;
    using Rml::Input::KI_X;
    using Rml::Input::KI_Y;
    using Rml::Input::KI_Z;
    using Rml::Input::KI_UP;

    if (key >= SDLK_a && key <= SDLK_z) {
        return static_cast<Rml::Input::KeyIdentifier>(KI_A + (key - SDLK_a));
    }

    if (key >= SDLK_0 && key <= SDLK_9) {
        return static_cast<Rml::Input::KeyIdentifier>(KI_0 + (key - SDLK_0));
    }

    switch (key) {
        case SDLK_ESCAPE:
            return KI_ESCAPE;
        case SDLK_RETURN:
            return KI_RETURN;
        case SDLK_BACKSPACE:
            return KI_BACK;
        case SDLK_TAB:
            return KI_TAB;
        case SDLK_SPACE:
            return KI_SPACE;
        case SDLK_LEFT:
            return KI_LEFT;
        case SDLK_RIGHT:
            return KI_RIGHT;
        case SDLK_UP:
            return KI_UP;
        case SDLK_DOWN:
            return KI_DOWN;
        case SDLK_DELETE:
            return KI_DELETE;
        default:
            return KI_UNKNOWN;
    }
}

auto RmlUiBackend::translate_modifiers(SDL_Keymod mods) -> int {
    int result = 0;

    if ((mods & KMOD_SHIFT) != 0) {
        result |= Rml::Input::KM_SHIFT;
    }
    if ((mods & KMOD_CTRL) != 0) {
        result |= Rml::Input::KM_CTRL;
    }
    if ((mods & KMOD_ALT) != 0) {
        result |= Rml::Input::KM_ALT;
    }
    if ((mods & KMOD_GUI) != 0) {
        result |= Rml::Input::KM_META;
    }

    return result;
}

}  // namespace engine::ui::backends::rml

