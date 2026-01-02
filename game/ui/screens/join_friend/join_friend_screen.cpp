#include "game/ui/screens/join_friend/join_friend_screen.hpp"

#include <string>
#include <utility>

namespace game::ui::join_friend {

namespace {

constexpr std::string_view kStartMenuScreenId = "start_menu";

}  // namespace

JoinFriendScreen::JoinFriendScreen(game::GameState& state,
                                   engine::backend::NetworkManager& network_manager,
                                   game::state::ChatStore& chat_store)
    : state_{&state},
      network_manager_{&network_manager},
      chat_store_{&chat_store} {}

auto JoinFriendScreen::id() const noexcept -> std::string_view {
    return kId;
}

auto JoinFriendScreen::build() -> engine::ui::UiScreenBuildResult {
    engine::ui::UiElement column{};
    column.tag = "div";
    column.classes = {"screen-content", "center-column"};

    game::ui::components::TitleComponent title("JOIN A FRIEND");
    column.children.push_back(title.render());

    column.children.push_back(build_status_label());

    game::ui::components::OptionListComponent list(
        game::ui::components::OptionListProps{
            .options = build_options()
        }
    );
    column.children.push_back(list.render());

    game::ui::components::MenuOptionComponent back_option(game::ui::components::MenuOptionProps{
        .id = "option-back",
        .label = "Back to Menu",
        .on_select = [this] { handle_back(); }
    });
    column.children.push_back(back_option.render());

    engine::ui::UiScreenBuildResult result{};
    result.root = std::move(column);
    result.template_path = "game/ui/screens/join_friend/join_friend_screen.rml";
    result.stylesheets.push_back("game/ui/screens/join_friend/join_friend_screen.rcss");

    auto title_styles = game::ui::components::TitleComponent::stylesheets();
    result.stylesheets.insert(result.stylesheets.end(), title_styles.begin(), title_styles.end());

    auto option_styles = game::ui::components::MenuOptionComponent::stylesheets();
    result.stylesheets.insert(result.stylesheets.end(), option_styles.begin(), option_styles.end());

    auto list_styles = game::ui::components::OptionListComponent::stylesheets();
    result.stylesheets.insert(result.stylesheets.end(), list_styles.begin(), list_styles.end());

    return result;
}

void JoinFriendScreen::on_attach(engine::ui::UiScreenHost& host) {
    UiScreen::on_attach(host);
    host_ = &host;
    last_chat_count_ = 0;
    if (chat_store_ != nullptr) {
        const auto snapshot = chat_store_->state();
        last_chat_count_ = snapshot.chats.size();
        chat_subscription_ = chat_store_->subscribe(
            [this](const game::state::ChatState& snapshot) {
                last_chat_count_ = snapshot.chats.size();
                dirty_ = true;
                if (host_ != nullptr) {
                    host_->mark_dirty(kId);
                }
            }
        );
        if (network_manager_ != nullptr && snapshot.chats.empty()) {
            network_manager_->request_chats(5);
        }
    }
}

void JoinFriendScreen::on_detach() {
    UiScreen::on_detach();
    if (chat_store_ != nullptr && chat_subscription_ != 0) {
        chat_store_->unsubscribe(chat_subscription_);
        chat_subscription_ = 0;
    }
    host_ = nullptr;
}

auto JoinFriendScreen::update(float /*dt*/) -> bool {
    const bool was_dirty = dirty_;
    dirty_ = false;
    return was_dirty;
}

auto JoinFriendScreen::build_options() -> std::vector<game::ui::components::MenuOptionProps> {
    std::vector<game::ui::components::MenuOptionProps> options{};

    if (chat_store_ == nullptr) {
        return options;
    }

    const auto chat_state = chat_store_->state();
    for (const auto& chat : chat_state.chats) {
        options.push_back(game::ui::components::MenuOptionProps{
            .id = "chat-" + std::to_string(chat.id),
            .label = chat.title,
            .on_select = [this, chat_id = chat.id] { handle_select_chat(chat_id); }
        });
    }

    if (options.empty()) {
        options.push_back(game::ui::components::MenuOptionProps{
            .id = "chat-empty",
            .label = "No chats available",
            .on_select = {}
        });
    }

    return options;
}

auto JoinFriendScreen::build_status_label() const -> engine::ui::UiElement {
    bool loading = false;
    bool has_chats = false;
    bool ready = false;

    if (chat_store_ != nullptr) {
        const auto chat_state = chat_store_->state();
        loading = chat_state.backend_connecting;
        has_chats = !chat_state.chats.empty();
        ready = chat_state.backend_ready && has_chats;
    }

    std::string text;
    if (loading) {
        text = "Loading chats...";
    } else if (ready) {
        text = "Select a chat to view history.";
    } else if (!loading && has_chats) {
        text = "Select a chat to view history.";
    } else {
        text = "No chats available.";
    }

    game::ui::components::LabelComponent label(game::ui::components::LabelProps{
        .id = "join-friend-status",
        .text = text,
        .font_size = 20,
        .variant_class = "subtitle"
    });
    return label.render();
}

void JoinFriendScreen::handle_select_chat(engine::backend::ChatId chat_id) {
    if (network_manager_ != nullptr) {
        network_manager_->request_history(chat_id, 10);
    }
}

void JoinFriendScreen::handle_back() {
    if (host_ == nullptr) {
        return;
    }

    host_->push_screen(kStartMenuScreenId);
    host_->pop_screen(kId);
}

}  // namespace game::ui::join_friend


