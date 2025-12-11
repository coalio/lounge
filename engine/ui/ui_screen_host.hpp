#pragma once

#include <string_view>

namespace engine::ui {

class UiScreenHost {
public:
    UiScreenHost() = default;
    UiScreenHost(const UiScreenHost&) = delete;
    auto operator=(const UiScreenHost&) -> UiScreenHost& = delete;
    UiScreenHost(UiScreenHost&&) = delete;
    auto operator=(UiScreenHost&&) -> UiScreenHost& = delete;
    virtual ~UiScreenHost() = default;

    virtual void push_screen(std::string_view id) = 0;
    virtual void pop_screen(std::string_view id) = 0;
    virtual void pop_top_screen() = 0;
    virtual void replace_screen(std::string_view id) = 0;
    virtual void mark_dirty(std::string_view id) = 0;
};

}  // namespace engine::ui


