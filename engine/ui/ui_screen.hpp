#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "engine/ui/ui_element.hpp"
#include "engine/ui/ui_screen_host.hpp"
#include "engine/ui/ui_types.hpp"

namespace engine::ui {

struct UiScreenBuildResult {
    UiElement root{};
    std::vector<std::string> stylesheets{};
    std::string template_path{};
};

class UiScreen {
public:
    UiScreen() = default;
    UiScreen(const UiScreen&) = default;
    UiScreen(UiScreen&&) = default;
    auto operator=(const UiScreen&) -> UiScreen& = default;
    auto operator=(UiScreen&&) -> UiScreen& = default;
    virtual ~UiScreen() = default;

    [[nodiscard]] virtual auto id() const noexcept -> std::string_view = 0;
    [[nodiscard]] virtual auto build() -> UiScreenBuildResult = 0;
    virtual void on_attach(UiScreenHost& host) {
        host_ = &host;
    }
    virtual void on_detach() {
        host_ = nullptr;
    }
    virtual auto update(float /*dt*/) -> bool {
        return false;
    }

protected:
    [[nodiscard]] auto host() const noexcept -> UiScreenHost* {
        return host_;
    }

private:
    UiScreenHost* host_{nullptr};
};

using UiScreenPtr = std::unique_ptr<UiScreen>;

}  // namespace engine::ui

