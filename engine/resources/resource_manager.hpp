#pragma once

namespace engine::resources {

class ResourceManager {
public:
    ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    auto operator=(const ResourceManager&) -> ResourceManager& = delete;
    ResourceManager(ResourceManager&&) = delete;
    auto operator=(ResourceManager&&) -> ResourceManager& = delete;
    ~ResourceManager() = default;
};

}  // namespace engine::resources


