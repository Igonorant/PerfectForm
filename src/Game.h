#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

#include "TextureManager.h"

namespace PF
{
class Object;
class Player;

/**
 * @brief Main game class responsible for managing game state and resources
 */
class Game
{
  public:
    explicit Game(SDL_Renderer* renderer);

    void update(Uint64 stepMs);

    void handleKeyboardEvent(SDL_Event* event);

    void render() const;

    PF::TextureManager& getTextureManager();
    const PF::TextureManager& getTextureManager() const;

  private:
    void initializePlayer();  // Initialize player object

  private:
    SDL_Renderer* m_renderer = nullptr;              // Pointer to the SDL renderer
    PF::TextureManager m_textureManager;             // Texture manager for handling textures
    std::vector<std::shared_ptr<Object>> m_objects;  // Collection of game objects
    std::shared_ptr<Player> m_player;                // Pointer to the player object.
};
}  // namespace PF
