#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "Object.h"
#include "TextureManager.h"

namespace PF
{
/**
 * @brief Main game class responsible for managing game state and resources
 */
class Game
{
  public:
    Game(SDL_Renderer* renderer);

    void update(Uint64 stepMs);

    void render() const;

    PF::TextureManager& getTextureManager();
    const PF::TextureManager& getTextureManager() const;

  private:
    void initializePlayer();  // Initialize player object

  private:
    SDL_Renderer* m_renderer = nullptr;              // Pointer to the SDL renderer
    PF::TextureManager m_textureManager;             // Texture manager for handling textures
    std::vector<std::unique_ptr<Object>> m_objects;  // Collection of game objects
};
}  // namespace PF
