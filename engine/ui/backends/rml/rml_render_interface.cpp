#include "engine/ui/backends/rml/rml_render_interface.hpp"

#include <SDL.h>

#include <vector>

namespace engine::ui::backends::rml {

namespace {

auto to_sdl_color(const Rml::ColourbPremultiplied& color) -> SDL_Color {
    const auto non_premultiplied = color.ToNonPremultiplied();
    return SDL_Color{
        non_premultiplied.red,
        non_premultiplied.green,
        non_premultiplied.blue,
        non_premultiplied.alpha
    };
}

}  // namespace

RmlRenderInterface::RmlRenderInterface(SDL_Renderer* renderer)
    : renderer_{renderer} {}

Rml::CompiledGeometryHandle RmlRenderInterface::CompileGeometry(
    Rml::Span<const Rml::Vertex> vertices,
    Rml::Span<const int> indices) {
    if (renderer_ == nullptr || vertices.empty() || indices.empty()) {
        return 0;
    }

    Geometry geometry{};
    geometry.vertices.assign(vertices.begin(), vertices.end());
    geometry.indices.assign(indices.begin(), indices.end());

    const auto handle = next_geometry_handle_++;
    geometries_.emplace(handle, std::move(geometry));
    return handle;
}

void RmlRenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry_handle,
                                        Rml::Vector2f translation,
                                        Rml::TextureHandle texture) {
    if (renderer_ == nullptr || geometry_handle == 0) {
        return;
    }

    const auto geometry_it = geometries_.find(geometry_handle);
    if (geometry_it == geometries_.end()) {
        return;
    }

    const auto& geometry = geometry_it->second;
    if (geometry.vertices.empty() || geometry.indices.empty()) {
        return;
    }

    std::vector<float> positions(geometry.vertices.size() * 2U);
    std::vector<SDL_Color> colors(geometry.vertices.size());
    std::vector<float> uvs(geometry.vertices.size() * 2U);

    for (std::size_t i = 0; i < geometry.vertices.size(); ++i) {
        const auto& vertex = geometry.vertices[i];
        positions[2U * i] = translation.x + vertex.position.x;
        positions[2U * i + 1U] = translation.y + vertex.position.y;
        colors[i] = to_sdl_color(vertex.colour);
        uvs[2U * i] = vertex.tex_coord.x;
        uvs[2U * i + 1U] = vertex.tex_coord.y;
    }

    SDL_Texture* sdl_texture = nullptr;
    if (texture != 0) {
        if (const auto texture_it = textures_.find(texture); texture_it != textures_.end()) {
            sdl_texture = texture_it->second;
        }
    }

    SDL_RenderGeometryRaw(
        renderer_,
        sdl_texture,
        positions.data(),
        sizeof(float) * 2,
        colors.data(),
        sizeof(SDL_Color),
        uvs.data(),
        sizeof(float) * 2,
        static_cast<int>(geometry.vertices.size()),
        geometry.indices.data(),
        static_cast<int>(geometry.indices.size()),
        sizeof(int)
    );
}

void RmlRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry_handle) {
    if (geometry_handle == 0) {
        return;
    }
    geometries_.erase(geometry_handle);
}

Rml::TextureHandle RmlRenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions,
                                                   const Rml::String& source) {
    SDL_Surface* surface = SDL_LoadBMP(source.c_str());
    if (surface == nullptr) {
        texture_dimensions = {0, 0};
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    texture_dimensions = {surface->w, surface->h};
    SDL_FreeSurface(surface);

    if (texture == nullptr) {
        return 0;
    }

    const auto handle = next_texture_handle_++;
    textures_.emplace(handle, texture);
    return handle;
}

Rml::TextureHandle RmlRenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source,
                                                       Rml::Vector2i source_dimensions) {
    if (renderer_ == nullptr || source.empty()) {
        return 0;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
        const_cast<Rml::byte*>(source.data()),
        source_dimensions.x,
        source_dimensions.y,
        32,
        source_dimensions.x * 4,
        SDL_PIXELFORMAT_ABGR8888
    );

    if (surface == nullptr) {
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);

    if (texture == nullptr) {
        return 0;
    }

    const auto handle = next_texture_handle_++;
    textures_.emplace(handle, texture);
    return handle;
}

void RmlRenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle) {
    if (texture_handle == 0) {
        return;
    }
    if (const auto it = textures_.find(texture_handle); it != textures_.end()) {
        SDL_DestroyTexture(it->second);
        textures_.erase(it);
    }
}

void RmlRenderInterface::EnableScissorRegion(bool enable) {
    if (!enable) {
        SDL_RenderSetClipRect(renderer_, nullptr);
    }
}

void RmlRenderInterface::SetScissorRegion(Rml::Rectanglei region) {
    const SDL_Rect rect{
        region.Left(),
        region.Top(),
        region.Width(),
        region.Height()
    };
    SDL_RenderSetClipRect(renderer_, &rect);
}

void RmlRenderInterface::SetTransform(const Rml::Matrix4f*) {}

}  // namespace engine::ui::backends::rml

