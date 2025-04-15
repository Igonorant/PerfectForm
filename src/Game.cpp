#include "Game.h"
#include "Player.h"

PF::Game::Game(SDL_Renderer* renderer): m_renderer(renderer), m_textureManager(renderer)
{
    // Initialize game objects
    initializePlayer();
}

void PF::Game::initializePlayer()
{
    SDL_FRect srcRect = {0, 0, 32, 32};
    SDL_FPoint position = {1280 / 2.0f, 720 / 2.0f};
    float size = 2.0f;
    const auto textureIdx = m_textureManager.addTexture("../../assets/BaseCell_32x32.png");
    m_objects.emplace_back(std::make_unique<PF::Player>(textureIdx, srcRect, position, size));
}

void PF::Game::update(Uint64 stepMs)
{
    for (auto& object : m_objects)
    {
        object->update(stepMs);
        // wait
    }
}

void PF::Game::render() const
{
    for (const auto& object : m_objects) { object->render(m_renderer, m_textureManager); }
}

PF::TextureManager& PF::Game::getTextureManager() { return m_textureManager; }

const PF::TextureManager& PF::Game::getTextureManager() const { return m_textureManager; }
