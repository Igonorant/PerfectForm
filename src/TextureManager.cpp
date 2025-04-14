#include <cassert>
#include <filesystem>
#include <stdexcept>

#include "TextureManager.h"

PF::Texture::Texture(SDL_Renderer* renderer, std::string_view filePath)
{
    std::filesystem::path canonicalPath = std::filesystem::canonical(filePath);
    SDL_Surface* fileSurface = IMG_Load(canonicalPath.string().c_str());
    if (!fileSurface)
    {
        const auto errorMsg =
            std::string("Couldn't load surface from file: ") + std::string(filePath) + "\n  Error: " + SDL_GetError();
        throw std::runtime_error(errorMsg);
    }
    m_texture = SDL_CreateTextureFromSurface(renderer, fileSurface);
    if (!m_texture)
    {
        const auto errorMsg = std::string("Couldn't create texture from surface: ") + std::string(filePath) +
                              "\n  Error: " + SDL_GetError();
        throw std::runtime_error(errorMsg);
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
    SDL_Log("Texture added from file: %s\n", filePath.data());
    return m_textures.size() - 1;  // Return the index of the added texture
}

const PF::Texture& PF::TextureManager::getTexture(std::size_t index) const
{
    if (index >= m_textures.size()) { throw std::out_of_range("Texture index out of range"); }
    return m_textures[index];
}
