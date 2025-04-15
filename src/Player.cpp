#include <math.h>

#include "Player.h"

PF::Player::Player(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size)
    : Object(textureIdx, srcRect, position, size)
{
}

void PF::Player::update(Uint64 stepMs)
{
    m_angle = (float)(stepMs * 0.0007f) + m_angle;  // Convert ms to seconds for smoother motion
    // float orbitX = sinf(m_angle) * 100.0f;          // Oscillates between -100.0f and 100.0f
    // float orbitY = cosf(m_angle) * 100.0f;          // Creates circular pattern when combined with scale_x

    // m_position.x = orbitX + 1280 / 2.0f;
    // m_position.y = orbitY + 720 / 2.0f;

    m_position.x += m_velocity.x;  // Update position based on velocity
    m_position.y += m_velocity.y;  // Update position based on velocity

    m_size = 1.0f + (sinf(m_angle * 7) * 0.05f);  // Scale between 0.95 and 1.05
}

void PF::Player::handleKeyboardEvent(SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_KEY_DOWN:
        {
            const float velocity = 2.0f;  // Set a constant velocity for movement
            switch (event->key.key)
            {
                case SDLK_UP: m_velocity.y = -velocity; break;
                case SDLK_DOWN: m_velocity.y = velocity; break;
                case SDLK_LEFT: m_velocity.x = -velocity; break;
                case SDLK_RIGHT: m_velocity.x = velocity; break;
                default: break;
            }
            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            switch (event->key.key)
            {
                case SDLK_UP: [[fallthrough]];
                case SDLK_DOWN: m_velocity.y = 0.0f; break;
                case SDLK_LEFT: [[fallthrough]];
                case SDLK_RIGHT: m_velocity.x = 0.0f; break;
                default: break;
            }
            break;
        }
        default: break;
    }
}
