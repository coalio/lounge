#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace engine::resources {

class ResourceManager {
public:
    explicit ResourceManager(std::filesystem::path root = std::filesystem::current_path());
    ResourceManager(const ResourceManager&) = delete;
    auto operator=(const ResourceManager&) -> ResourceManager& = delete;
    ResourceManager(ResourceManager&&) = delete;
    auto operator=(ResourceManager&&) -> ResourceManager& = delete;
    ~ResourceManager() = default;

    [[nodiscard]] auto load_text(std::string_view relative_path)
        -> std::expected<std::string_view, std::string>;

    [[nodiscard]] auto resolve(std::string_view relative_path) const -> std::filesystem::path;

private:
    std::filesystem::path root_{};
    std::unordered_map<std::string, std::string> text_cache_{};
};

}  // namespace engine::resources


