#include "engine/resources/resource_manager.hpp"

#include <fstream>
#include <sstream>
#include <utility>

namespace engine::resources {

ResourceManager::ResourceManager(std::filesystem::path root)
    : root_{std::move(root)} {}

auto ResourceManager::load_text(std::string_view relative_path)
    -> std::expected<std::string_view, std::string> {
    const std::string key(relative_path);

    if (const auto cached = text_cache_.find(key); cached != text_cache_.end()) {
        return std::string_view{cached->second};
    }

    const auto path = resolve(relative_path);
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        return std::unexpected("Failed to open resource: " + path.string());
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    if (file.bad()) {
        return std::unexpected("Failed to read resource: " + path.string());
    }

    auto [iter, _] = text_cache_.emplace(key, buffer.str());
    return std::string_view{iter->second};
}

auto ResourceManager::resolve(std::string_view relative_path) const -> std::filesystem::path {
    return root_ / relative_path;
}

}  // namespace engine::resources
