#include <math.h>

#include "Player.h"

PF::Player::Player(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size)
    : Object(textureIdx, srcRect, position, size)
{
}

void PF::Player::update(Uint64 stepMs)
{
    m_angle = (float)(stepMs * 0.0007f) + m_angle;
    // float orbitX = sinf(m_angle) * 100.0f;          // Oscillates between -100.0f and 100.0f
    // float orbitY = cosf(m_angle) * 100.0f;          // Creates circular pattern when combined with scale_x

    // m_position.x = orbitX + 1280 / 2.0f;
    // m_position.y = orbitY + 720 / 2.0f;

    m_position.x += m_velocity.x;  // Update position based on velocity
    m_position.y += m_velocity.y;  // Update position based on velocity

    m_size = 1.0f + (sinf(m_angle * 7) * 0.05f);  // Scale between 0.95 and 1.05
}

std::shared_ptr<PF::Object> PF::Player::handleKeyboardEvent(SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_KEY_DOWN:
        {
            const float velocity = 2.0f;  // Set a constant velocity for movement
            switch (event->key.key)
            {
                case SDLK_UP:
                {
                    m_velocity.y = -velocity;
                    break;
                }
                case SDLK_DOWN:
                {
                    m_velocity.y = velocity;
                    break;
                }
                case SDLK_LEFT:
                {
                    m_velocity.x = -velocity;
                    break;
                }
                case SDLK_RIGHT:
                {
                    m_velocity.x = velocity;
                    break;
                }
                default: break;
            }
            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            switch (event->key.key)
            {
                case SDLK_Q:
                {
                    return spawnAttack();
                }
                case SDLK_UP: [[fallthrough]];
                case SDLK_DOWN:
                {
                    m_velocity.y = 0.0f;
                    break;
                }
                case SDLK_LEFT: [[fallthrough]];
                case SDLK_RIGHT:
                {
                    m_velocity.x = 0.0f;
                    break;
                }
                default: break;
            }
            break;
        }
        default: break;
    }
    return {};
}

std::shared_ptr<PF::Object> PF::Player::spawnAttack() const
{
    const float velocitySum = m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y;
    if (velocitySum < 0.001f) { return {}; }  // No movement, no attack

    float size = m_size * 0.3333f;
    auto attack = std::make_shared<PF::Attack>(m_textureIdx, m_srcRect, m_position, size);

    SDL_FPoint velocity = m_velocity;
    velocity.x *= 2.0f;
    velocity.y *= 2.0f;
    attack->setVelocity(velocity);  // Set the velocity of the attack
    return attack;
}

PF::Attack::Attack(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size)
    : Object(textureIdx, srcRect, position, size)
{
}

void PF::Attack::update(Uint64 stepMs)
{
    m_angle = (float)(stepMs * 0.005f) + m_angle;
    float sinAngle = sinf(m_angle);
    float cosAngle = cosf(1.33f * m_angle);

    m_position.x += m_velocity.x * (1 + sinAngle) + 0.5f * sinAngle;  // Update position based on velocity
    m_position.y += m_velocity.y * (1 + cosAngle) + 0.5f * cosAngle;  // Update position based on velocity

    // Decelerate x velocity based on proportion of x/y velocity
    if (m_velocity.x > 0.001f) { m_velocity.x -= m_deceleration; }
    else if (m_velocity.x < -0.001f) { m_velocity.x += m_deceleration; }
    else { m_velocity.x = 0.0f; }

    // Decelerate y velocity based on inverse proportion of x/y velocity
    if (m_velocity.y > 0.001f) { m_velocity.y -= m_deceleration; }
    else if (m_velocity.y < -0.001f) { m_velocity.y += m_deceleration; }
    else { m_velocity.y = 0.0f; }

    m_size = m_size + (sinf(m_angle * 3) * 0.005f) - 0.00005f * m_angle;
}

void PF::Attack::setVelocity(SDL_FPoint velocity) { m_velocity = velocity; }

bool PF::Attack::shouldRemove() const { return m_size < 0.02f; }
