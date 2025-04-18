#include <math.h>

#include "Player.h"

constexpr float ANGLE_INCREMENT = 0.0007F;
constexpr float SCALE_FACTOR = 0.05F;
constexpr float VELOCITY = 2.0F;
constexpr float MIN_VELOCITY_THRESHOLD = 0.001F;
constexpr float ATTACK_SIZE_FACTOR = 0.3333F;
constexpr float ATTACK_VELOCITY_MULTIPLIER = 2.0F;
constexpr float ATTACK_DECELERATION = 0.025F;
constexpr float COS_ANGLE_MULTIPLIER = 1.33F;
constexpr float POSITION_OFFSET = 0.5F;

PF::Player::Player(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size)
    : Object(textureIdx, srcRect, position, size)
{
}

void PF::Player::update(Uint64 stepMs)
{
    m_angle += stepMs * ANGLE_INCREMENT;
    m_position.x += m_velocity.x;  // Update position based on velocity
    m_position.y += m_velocity.y;  // Update position based on velocity

    m_size = 1.0F + (sinf(m_angle * 7) * SCALE_FACTOR);  // Scale between 0.95 and 1.05
}

std::shared_ptr<PF::Object> PF::Player::handleKeyboardEvent(SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_KEY_DOWN:
        {
            switch (event->key.key)
            {
                case SDLK_UP:
                {
                    m_velocity.y = -VELOCITY;
                    break;
                }
                case SDLK_DOWN:
                {
                    m_velocity.y = VELOCITY;
                    break;
                }
                case SDLK_LEFT:
                {
                    m_velocity.x = -VELOCITY;
                    break;
                }
                case SDLK_RIGHT:
                {
                    m_velocity.x = VELOCITY;
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
    if (velocitySum < MIN_VELOCITY_THRESHOLD) { return {}; }  // No movement, no attack

    float size = m_size * ATTACK_SIZE_FACTOR;
    auto attack = std::make_shared<PF::Attack>(m_textureIdx, m_srcRect, m_position, size);

    SDL_FPoint velocity = m_velocity;
    velocity.x *= ATTACK_VELOCITY_MULTIPLIER;
    velocity.y *= ATTACK_VELOCITY_MULTIPLIER;
    attack->setVelocity(velocity);  // Set the velocity of the attack
    return attack;
}

PF::Attack::Attack(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size)
    : Object(textureIdx, srcRect, position, size)
{
}

void PF::Attack::update(Uint64 stepMs)
{
    m_angle += stepMs * 0.005f;
    float sinAngle = sinf(m_angle);
    float cosAngle = cosf(COS_ANGLE_MULTIPLIER * m_angle);

    m_position.x += m_velocity.x * (1 + sinAngle) + POSITION_OFFSET * sinAngle;  // Update position based on velocity
    m_position.y += m_velocity.y * (1 + cosAngle) + POSITION_OFFSET * cosAngle;  // Update position based on velocity

    // Decelerate x velocity based on proportion of x/y velocity
    if (m_velocity.x > MIN_VELOCITY_THRESHOLD) { m_velocity.x -= ATTACK_DECELERATION; }
    else if (m_velocity.x < -MIN_VELOCITY_THRESHOLD) { m_velocity.x += ATTACK_DECELERATION; }
    else { m_velocity.x = 0.0f; }

    // Decelerate y velocity based on inverse proportion of x/y velocity
    if (m_velocity.y > MIN_VELOCITY_THRESHOLD) { m_velocity.y -= ATTACK_DECELERATION; }
    else if (m_velocity.y < -MIN_VELOCITY_THRESHOLD) { m_velocity.y += ATTACK_DECELERATION; }
    else { m_velocity.y = 0.0f; }

    m_size = m_size + (sinf(m_angle * 3) * 0.005f) - 0.00005f * m_angle;
}

void PF::Attack::setVelocity(SDL_FPoint velocity) { m_velocity = velocity; }

bool PF::Attack::shouldRemove() const { return m_size < 0.02f; }
