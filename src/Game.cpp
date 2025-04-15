#include "Game.h"
#include "Object.h"
#include "Player.h"

PF::Game::Game(SDL_Renderer* renderer): m_renderer(renderer), m_textureManager(renderer)
{
    // Initialize game objects
    initializePlayer();
}

void PF::Game::initializePlayer()
{
    SDL_FRect srcRect = {0, 0, 64, 64};
    SDL_FPoint position = {1280 / 2.0f, 720 / 2.0f};
    float size = 1.0f;
    const auto textureIdx = m_textureManager.addTexture("../../assets/BaseCell_64x64.png");
    m_player = std::make_shared<PF::Player>(textureIdx, srcRect, position, size);
    m_objects.emplace_back(m_player);
}

void PF::Game::update(Uint64 stepMs)
{
    for (auto& object : m_objects) { object->update(stepMs); }
}

void PF::Game::handleKeyboardEvent(SDL_Event* event)
{
    for (auto& object : m_objects) { object->handleKeyboardEvent(event); }
    switch (event->type)
    {
        case SDL_EVENT_KEY_DOWN:
        {
            SDL_Log("Key down event detected.");
            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            SDL_Log("Key up event detected.");
            break;
        }
        default: break;
    }
}

void PF::Game::render() const
{
    for (const auto& object : m_objects) { object->render(m_renderer, m_textureManager); }
}

PF::TextureManager& PF::Game::getTextureManager() { return m_textureManager; }

const PF::TextureManager& PF::Game::getTextureManager() const { return m_textureManager; }
