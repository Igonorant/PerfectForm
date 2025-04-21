#include <cassert>
#include <cstddef>
#include <filesystem>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>

#include "Exceptions.h"
#include "TextureManager.h"

PF::Texture::Texture(SDL_Renderer* renderer, std::string_view filePath)
{
    std::filesystem::path canonicalPath = std::filesystem::canonical(filePath);
    SDL_Surface* fileSurface = IMG_Load(canonicalPath.string().c_str());
    if (fileSurface == nullptr) { throw PF::SDLException(std::format("Couldn't load image file: {}", filePath)); }

    m_texture = SDL_CreateTextureFromSurface(renderer, fileSurface);
    if (m_texture == nullptr)
    {
        throw PF::SDLException(std::format("Couldn't create texture from surface: {}", filePath));
    }
    SDL_DestroySurface(fileSurface);  // done with this, the texture has a copy of the pixels now.
}

SDL_Texture& PF::Texture::get() const
{
    assert(m_texture);
    return *m_texture;
}

SDL_Texture& PF::Texture::operator->() const { return get(); }

SDL_Texture& PF::Texture::operator*() const { return get(); }

PF::TextureManager::TextureManager(SDL_Renderer* renderer): m_renderer(renderer) {}

std::size_t PF::TextureManager::addTexture(std::string_view filePath)
{
    m_textures.emplace_back(m_renderer, filePath);
    SDL_Log("Texture added from file: %s\n", std::string{filePath}.c_str());
    return m_textures.size() - 1;  // Return the index of the added texture
}

const PF::Texture& PF::TextureManager::getTexture(std::size_t index) const
{
    if (index >= m_textures.size()) { throw std::out_of_range("Texture index out of range"); }
    return m_textures[index];
}
