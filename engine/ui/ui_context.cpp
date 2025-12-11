#include "engine/ui/ui_context.hpp"

#include <iostream>
#include <unordered_set>
#include <utility>

#include "engine/ui/ui_document.hpp"

namespace engine::ui {

namespace {

auto dedupe(std::vector<std::string> values) -> std::vector<std::string> {
    std::vector<std::string> result{};
    std::unordered_set<std::string> seen{};
    result.reserve(values.size());

    for (auto& value : values) {
        if (seen.insert(value).second) {
            result.push_back(std::move(value));
        }
    }

    return result;
}

}  // namespace

UiContext::UiContext(UiBackend& backend, engine::resources::ResourceManager& resources)
    : backend_{&backend},
      resources_{&resources} {}

void UiContext::register_screen(std::string_view id, UiScreenRegistry::Factory factory) {
    registry_.register_screen(id, std::move(factory));
}

void UiContext::set_global_styles(std::vector<std::string> styles) {
    global_styles_ = dedupe(std::move(styles));
    for (auto& screen : screen_stack_) {
        screen.dirty = true;
    }
    documents_dirty_ = true;
}

void UiContext::request_screen(std::string_view id) {
    enqueue_command(ScreenCommandType::ReplaceAll, id);
}

void UiContext::push_screen(std::string_view id) {
    enqueue_command(ScreenCommandType::Push, id);
}

void UiContext::pop_screen(std::string_view id) {
    enqueue_command(ScreenCommandType::Pop, id);
}

void UiContext::pop_top_screen() {
    enqueue_command(ScreenCommandType::PopTop, {});
}

void UiContext::replace_screen(std::string_view id) {
    request_screen(id);
}

void UiContext::mark_dirty(std::string_view id) {
    for (auto& screen : screen_stack_) {
        if (screen.id == id) {
            screen.dirty = true;
            documents_dirty_ = true;
        }
    }
}

void UiContext::update(float dt) {
    process_commands();

    for (auto it = screen_stack_.begin(); it != screen_stack_.end();) {
        auto& active = *it;
        if (active.screen == nullptr) {
            it = screen_stack_.erase(it);
            documents_dirty_ = true;
            continue;
        }

        if (active.screen->update(dt)) {
            active.dirty = true;
        }

        if (active.dirty) {
            if (!rebuild_screen(active)) {
                active.screen->on_detach();
                it = screen_stack_.erase(it);
                documents_dirty_ = true;
                continue;
            }
            documents_dirty_ = true;
        }

        ++it;
    }

    if (documents_dirty_) {
        rebuild_documents();
        documents_dirty_ = false;
    }

    backend_->update(dt);
}

void UiContext::render() {
    backend_->render();
}

void UiContext::process_event(const SDL_Event& event) {
    backend_->process_event(event);
}

void UiContext::enqueue_command(ScreenCommandType type, std::string_view id) {
    pending_commands_.push_back(ScreenCommand{
        .type = type,
        .id = ScreenId{id}
    });
}

void UiContext::process_commands() {
    if (pending_commands_.empty()) {
        return;
    }

    for (const auto& command : pending_commands_) {
        switch (command.type) {
            case ScreenCommandType::ReplaceAll:
                for (auto& screen : screen_stack_) {
                    if (screen.screen != nullptr) {
                        screen.screen->on_detach();
                    }
                }
                screen_stack_.clear();
                documents_dirty_ = true;
                push_new_screen(command.id);
                break;
            case ScreenCommandType::Push:
                push_new_screen(command.id);
                break;
            case ScreenCommandType::Pop:
                for (std::size_t i = screen_stack_.size(); i-- > 0;) {
                    if (screen_stack_[i].id == command.id) {
                        remove_screen_by_index(i);
                        break;
                    }
                }
                break;
            case ScreenCommandType::PopTop:
                if (!screen_stack_.empty()) {
                    remove_screen_by_index(screen_stack_.size() - 1);
                }
                break;
        }
    }

    pending_commands_.clear();
}

auto UiContext::push_new_screen(std::string_view id) -> bool {
    auto screen = registry_.create(id);
    if (!screen) {
        std::cerr << "UiScreen not registered: " << id << std::endl;
        return false;
    }

    screen->on_attach(*this);

    ActiveScreen entry{};
    entry.id = ScreenId{id};
    entry.screen = std::move(screen);
    entry.dirty = true;
    screen_stack_.push_back(std::move(entry));
    documents_dirty_ = true;
    return true;
}

void UiContext::rebuild_documents() {
    std::vector<UiDocument> documents{};
    documents.reserve(screen_stack_.size());
    for (const auto& screen : screen_stack_) {
        documents.push_back(screen.document);
    }
    backend_->sync_documents(documents);
}

auto UiContext::rebuild_screen(ActiveScreen& screen) -> bool {
    auto build_result = screen.screen->build();
    std::vector<std::string> style_paths = global_styles_;
    style_paths.insert(
        style_paths.end(),
        build_result.stylesheets.begin(),
        build_result.stylesheets.end()
    );
    style_paths = dedupe(std::move(style_paths));

    const auto style_contents = load_styles(style_paths);
    if (style_contents.size() != style_paths.size()) {
        std::cerr << "Failed to load styles for screen: " << screen.id << std::endl;
        return false;
    }

    std::string_view template_markup{};
    if (!build_result.template_path.empty()) {
        auto tpl = resources_->load_text(build_result.template_path);
        if (!tpl.has_value()) {
            std::cerr << tpl.error() << std::endl;
            return false;
        }
        template_markup = tpl.value();
    }

    screen.document = build_ui_document(
        std::move(build_result.root),
        style_contents,
        template_markup
    );
    screen.dirty = false;
    return true;
}

void UiContext::remove_screen_by_index(std::size_t index) {
    if (index >= screen_stack_.size()) {
        return;
    }

    auto& screen = screen_stack_[index];
    if (screen.screen != nullptr) {
        screen.screen->on_detach();
    }
    screen_stack_.erase(screen_stack_.begin() + static_cast<std::ptrdiff_t>(index));
    documents_dirty_ = true;
}

auto UiContext::load_styles(const std::vector<std::string>& specific)
    -> std::vector<std::string_view> {
    std::vector<std::string_view> result{};
    result.reserve(specific.size());

    for (const auto& path : specific) {
        auto css = resources_->load_text(path);
        if (!css.has_value()) {
            std::cerr << css.error() << std::endl;
            continue;
        }
        result.push_back(css.value());
    }

    return result;
}

}  // namespace engine::ui


