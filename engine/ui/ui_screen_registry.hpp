#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "engine/ui/ui_screen.hpp"

namespace engine::ui {

class UiScreenRegistry {
public:
    using Factory = std::function<UiScreenPtr()>;

    UiScreenRegistry() = default;
    UiScreenRegistry(const UiScreenRegistry&) = delete;
    auto operator=(const UiScreenRegistry&) -> UiScreenRegistry& = delete;
    UiScreenRegistry(UiScreenRegistry&&) = delete;
    auto operator=(UiScreenRegistry&&) -> UiScreenRegistry& = delete;
    ~UiScreenRegistry() = default;

    void register_screen(std::string_view id, Factory factory);
    [[nodiscard]] auto create(std::string_view id) const -> UiScreenPtr;

private:
    std::unordered_map<std::string, Factory> factories_{};
};

}  // namespace engine::ui

