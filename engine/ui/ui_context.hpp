#pragma once

#include <SDL.h>

#include <string>
#include <string_view>
#include <vector>

#include "engine/resources/resource_manager.hpp"
#include "engine/ui/ui_backend.hpp"
#include "engine/ui/ui_screen_host.hpp"
#include "engine/ui/ui_screen_registry.hpp"
#include "engine/ui/ui_types.hpp"

namespace engine::ui {

class UiContext : public UiScreenHost {
public:
    UiContext(UiBackend& backend, engine::resources::ResourceManager& resources);
    UiContext(const UiContext&) = delete;
    auto operator=(const UiContext&) -> UiContext& = delete;
    UiContext(UiContext&&) = delete;
    auto operator=(UiContext&&) -> UiContext& = delete;
    ~UiContext() = default;

    void register_screen(std::string_view id, UiScreenRegistry::Factory factory);
    void set_global_styles(std::vector<std::string> styles);
    void request_screen(std::string_view id);
    void push_screen(std::string_view id) override;
    void pop_screen(std::string_view id) override;
    void pop_top_screen() override;
    void replace_screen(std::string_view id) override;
    void mark_dirty(std::string_view id) override;

    void update(float dt);
    void render();
    void process_event(const SDL_Event& event);

private:
    enum class ScreenCommandType { ReplaceAll, Push, Pop, PopTop };
    struct ScreenCommand {
        ScreenCommandType type{ScreenCommandType::ReplaceAll};
        ScreenId id{};
    };

    struct ActiveScreen {
        ScreenId id{};
        UiScreenPtr screen{};
        UiDocument document{};
        bool dirty{true};
    };

    void enqueue_command(ScreenCommandType type, std::string_view id);
    void process_commands();
    auto push_new_screen(std::string_view id) -> bool;
    void rebuild_documents();
    auto rebuild_screen(ActiveScreen& screen) -> bool;
    void remove_screen_by_index(std::size_t index);
    auto load_styles(const std::vector<std::string>& specific) -> std::vector<std::string_view>;

    UiBackend* backend_{nullptr};
    engine::resources::ResourceManager* resources_{nullptr};
    UiScreenRegistry registry_{};
    std::vector<std::string> global_styles_{};
    std::vector<ActiveScreen> screen_stack_{};
    std::vector<ScreenCommand> pending_commands_{};
    bool documents_dirty_{false};
};

}  // namespace engine::ui

