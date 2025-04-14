#pragma once

#include <SDL3/SDL.h>

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

    PF::TextureManager& getTextureManager();
    const PF::TextureManager& getTextureManager() const;

  private:
    SDL_Renderer* m_renderer = nullptr;   // Pointer to the SDL renderer
    PF::TextureManager m_textureManager;  // Texture manager for handling textures
};
}  // namespace PF
