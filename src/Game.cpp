#include "Game.h"

PF::Game::Game(SDL_Renderer* renderer): m_renderer(renderer), m_textureManager(renderer) {}

PF::TextureManager& PF::Game::getTextureManager() { return m_textureManager; }

const PF::TextureManager& PF::Game::getTextureManager() const { return m_textureManager; }
