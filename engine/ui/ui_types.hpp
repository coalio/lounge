#pragma once

#include <string>
#include <string_view>

namespace engine::ui {

using ScreenId = std::string;

inline constexpr std::string_view kScreenContentToken = "{{SCREEN_CONTENT}}";

}  // namespace engine::ui

