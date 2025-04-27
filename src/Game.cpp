#include <iterator>
#include <memory>
#include <vector>

#include "Enums.h"
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
    // Texture source rectangle
    const float playerSrcWidth = 64.0F;
    const float playerSrcHeight = 64.0F;
    SDL_FRect srcRect = {0, 0, playerSrcWidth, playerSrcHeight};

    // Starting position
    const float playerStartX = 1280 / 2.0F;
    const float playerStartY = 720 / 2.0F;
    SDL_FPoint position = {playerStartX, playerStartY};

    // Starting size
    float startSize = 1.0F;

    // Load texture
    const auto textureIdx = m_textureManager.addTexture("../../assets/BaseCell_64x64.png");

    // Create player object
    m_player = std::make_shared<PF::Player>(textureIdx, srcRect, position, startSize);
    m_objects.emplace_back(m_player);
}

void PF::Game::update(Uint64 stepMs)
{
    // Update all game objects
    for (auto& object : m_objects) { object->update(stepMs); }

    // Remove objects that should be removed after updating
    std::erase_if(m_objects, [](const auto& object) { return object->shouldRemove(); });

    // Spawn new objects based on the current objects
    std::vector<std::shared_ptr<PF::Object>> newObjects;
    for (auto& object : m_objects)
    {
        auto newObject = object->spawnChildObject();
        if (newObject) { newObjects.emplace_back(newObject); }
    }
    m_objects.insert(
        m_objects.end(), std::make_move_iterator(newObjects.begin()), std::make_move_iterator(newObjects.end()));
}

void PF::Game::handleEvent(SDL_Event* event)
{
    const auto playerIntention = getPlayerIntention(event);

    for (auto& object : m_objects) { object->handleEvent(playerIntention); }
}

void PF::Game::render() const
{
    for (const auto& object : m_objects) { object->render(m_renderer, m_textureManager); }
}

namespace
{
void LogIntentionFromEvent(SDL_Event* event, const PF::PlayerIntention playerIntention)
{
    // Log the player intention and the event type
    SDL_Log("Player intention: %s - from SDL_Event: %s | 0x%x",
            PF::toString(playerIntention),
            SDL_GetKeyName(event->key.key),
            event->type);
}
}  // namespace

PF::PlayerIntention PF::Game::getPlayerIntention(SDL_Event* event)
{
    PF::PlayerIntention playerIntention = PF::PlayerIntention::NONE;
    switch (event->type)
    {
        case SDL_EVENT_KEY_DOWN:
        {
            switch (event->key.key)
            {
                case SDLK_UP: playerIntention = PF::PlayerIntention::MOVE_UP; break;
                case SDLK_DOWN: playerIntention = PF::PlayerIntention::MOVE_DOWN; break;
                case SDLK_LEFT: playerIntention = PF::PlayerIntention::MOVE_LEFT; break;
                case SDLK_RIGHT: playerIntention = PF::PlayerIntention::MOVE_RIGHT; break;
                case SDLK_Q: playerIntention = PF::PlayerIntention::ATTACK; break;
                default: break;
            }
            LogIntentionFromEvent(event, playerIntention);
            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            switch (event->key.key)
            {
                case SDLK_UP: playerIntention = PF::PlayerIntention::MOVE_STOP_UP; break;
                case SDLK_DOWN: playerIntention = PF::PlayerIntention::MOVE_STOP_DOWN; break;
                case SDLK_LEFT: playerIntention = PF::PlayerIntention::MOVE_STOP_LEFT; break;
                case SDLK_RIGHT: playerIntention = PF::PlayerIntention::MOVE_STOP_RIGHT; break;
                case SDLK_Q: playerIntention = PF::PlayerIntention::ATTACK_STOP; break;
                default: break;
            }
            LogIntentionFromEvent(event, playerIntention);
            break;
        }
        default: break;
    }

    return playerIntention;
}

PF::TextureManager& PF::Game::getTextureManager() { return m_textureManager; }

const PF::TextureManager& PF::Game::getTextureManager() const { return m_textureManager; }
