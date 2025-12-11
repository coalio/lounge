#include "game/ui/screens/start_menu/start_menu_screen.hpp"

#include <iostream>
#include <utility>

namespace game::ui::start_menu {

namespace {

constexpr std::string_view kGameplayScreenId = "gameplay";

}  // namespace

StartMenuScreen::StartMenuScreen(game::GameState& state)
    : state_{&state} {}

auto StartMenuScreen::id() const noexcept -> std::string_view {
    return kId;
}

auto StartMenuScreen::build() -> engine::ui::UiScreenBuildResult {
    engine::ui::UiElement column{};
    column.tag = "div";
    column.classes = {"screen-content", "center-column"};

    game::ui::components::TitleComponent title("LOUNGE");
    column.children.push_back(title.render());

    game::ui::components::OptionListComponent list(
        game::ui::components::OptionListProps{
            .options = build_options()
        }
    );
    column.children.push_back(list.render());

    engine::ui::UiScreenBuildResult result{};
    result.root = std::move(column);
    result.template_path = "game/ui/screens/start_menu/start_menu_screen.rml";
    result.stylesheets.push_back("game/ui/screens/start_menu/start_menu_screen.rcss");

    auto title_styles = game::ui::components::TitleComponent::stylesheets();
    result.stylesheets.insert(result.stylesheets.end(), title_styles.begin(), title_styles.end());

    auto option_styles = game::ui::components::MenuOptionComponent::stylesheets();
    result.stylesheets.insert(result.stylesheets.end(), option_styles.begin(), option_styles.end());

    auto list_styles = game::ui::components::OptionListComponent::stylesheets();
    result.stylesheets.insert(result.stylesheets.end(), list_styles.begin(), list_styles.end());

    return result;
}

void StartMenuScreen::on_attach(engine::ui::UiScreenHost& host) {
    UiScreen::on_attach(host);
    host_ = &host;
}

void StartMenuScreen::on_detach() {
    UiScreen::on_detach();
    host_ = nullptr;
}

void StartMenuScreen::handle_join_friend() {
    if (state_ != nullptr) {
        state_->gameplay_active = true;
    }

    if (host_ != nullptr) {
        host_->push_screen(kGameplayScreenId);
        host_->pop_screen(kId);
    }
}

auto StartMenuScreen::build_options() -> std::vector<game::ui::components::MenuOptionProps> {
    std::vector<game::ui::components::MenuOptionProps> options{};
    options.push_back(game::ui::components::MenuOptionProps{
        .id = "option-join-friend",
        .label = "Join a Friend",
        .on_select = [this] { handle_join_friend(); }
    });

    options.push_back(game::ui::components::MenuOptionProps{
        .id = "option-build-lounge",
        .label = "Build Your Lounge",
        .on_select =
            [] {
                std::cout << "Menu option selected: Build Your Lounge" << std::endl;
            }
    });

    return options;
}

}  // namespace game::ui::start_menu

