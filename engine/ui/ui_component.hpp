#pragma once

#include <utility>

#include "engine/ui/ui_element.hpp"

namespace engine::ui {

class UiComponent {
public:
    UiComponent() = default;
    UiComponent(const UiComponent&) = default;
    UiComponent(UiComponent&&) = default;
    auto operator=(const UiComponent&) -> UiComponent& = default;
    auto operator=(UiComponent&&) -> UiComponent& = default;
    virtual ~UiComponent() = default;

    virtual auto render() -> UiElement = 0;
};

template <typename Props>
class Component : public UiComponent {
public:
    explicit Component<Props>(Props props) : props_{std::move(props)} {}

protected:
    [[nodiscard]] auto props() const noexcept -> const Props& {
        return props_;
    }

private:
    Props props_{};
};

}  // namespace engine::ui

