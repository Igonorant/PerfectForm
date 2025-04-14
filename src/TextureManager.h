#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <string_view>
#include <vector>

namespace PF
{
/**
 * @class Texture
 * @brief Represents a texture loaded from an image file and managed by SDL.
 *
 * This class encapsulates the creation of an SDL_Texture from an image file.
 * It ensures proper resource management and provides a simple interface
 * for loading textures.
 */
class Texture
{
  public:
    /**
     * @brief Constructs a Texture object by loading an image file.
     * @param renderer The SDL_Renderer used to create the texture.
     * @param filePath The path to the image file to load.
     * @throws std::runtime_error if the image file cannot be loaded or the texture cannot be created.
     */
    Texture(SDL_Renderer* renderer, std::string_view filePath);

    SDL_Texture& get() const;
    SDL_Texture& operator->() const;
    SDL_Texture& operator*() const;

  private:
    SDL_Texture* m_texture = nullptr; /**< The SDL_Texture managed by this class. */
};

/**
 * @class TextureManager
 * @brief Manages a collection of textures.
 *
 * This class provides functionality to load and store multiple textures.
 * It allows adding textures by specifying their file paths and keeps track
 * of all loaded textures.
 */
class TextureManager
{
  public:
    /**
     * @brief Constructs a TextureManager object.
     * @param renderer The SDL_Renderer used to create textures.
     */
    TextureManager(SDL_Renderer* renderer);

    /**
     * @brief Adds a texture to the manager by loading it from a file.
     * @param renderer The SDL_Renderer used to create the texture.
     * @param filePath The path to the image file to load.
     * @return The index of the added texture in the internal collection.
     */
    std::size_t addTexture(std::string_view filePath);

    /**
     * @brief Retrieves the texture at the specified index.
     * @param index The index of the texture to retrieve.
     * @return A reference to the texture at the specified index.
     * @throws std::out_of_range if the index is invalid.
     */
    const Texture& getTexture(std::size_t index) const;

  private:
    SDL_Renderer* m_renderer;
    std::vector<Texture> m_textures; /**< Collection of loaded textures. */
};
}  // namespace PF
