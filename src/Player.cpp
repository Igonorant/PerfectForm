#include <cmath>
#include <cstddef>
#include <memory>

#include "Enums.h"
#include "Object.h"
#include "Player.h"

constexpr float ANGLE_INCREMENT = 0.0007F;
constexpr float ATTACK_ANGLE_INCREMENT = 0.005F;
constexpr float SCALE_FACTOR = 0.05F;
constexpr float SCALE_ANGLE_MULTIPLIER = 7.0F;
constexpr float VELOCITY = 2.0F;
constexpr float MIN_VELOCITY_THRESHOLD = 0.001F;
constexpr float ATTACK_SIZE_FACTOR = 0.3333F;
constexpr float ATTACK_VELOCITY_MULTIPLIER = 2.0F;
constexpr float ATTACK_DECELERATION = 0.025F;
constexpr float COS_ANGLE_MULTIPLIER = 1.33F;
constexpr float POSITION_OFFSET = 0.5F;
constexpr float ATTACK_SIZE_OSCILLATION = 0.005F;
constexpr float ATTACK_SIZE_DECAY = 0.00005F;
constexpr float MIN_ATTACK_SIZE = 0.02F;
constexpr float DIAGONAL_FACTOR = 0.7071F;  // 1/sqrt(2) for diagonal movement

PF::Player::Player(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size)
    : Object(textureIdx, srcRect, position, size)
{
}

void PF::Player::update(Uint64 stepMs)
{
    // Update velocity based on the current state
    switch (m_state)
    {
        case State::IDLE:
        {
            const float velocitySum = (m_velocity.x * m_velocity.x) + (m_velocity.y * m_velocity.y);
            if (velocitySum >= VELOCITY)
            {
                // Store the last velocity before stopping
                m_lastVelocity = m_velocity;
            }

            m_velocity.x = 0.0F;
            m_velocity.y = 0.0F;
            break;
        }
        case State::MOVING_UP:
        {
            m_velocity.x = 0.0F;
            m_velocity.y = -VELOCITY;
            break;
        }
        case State::MOVING_DOWN:
        {
            m_velocity.x = 0.0F;
            m_velocity.y = VELOCITY;
            break;
        }
        case State::MOVING_LEFT:
        {
            m_velocity.x = -VELOCITY;
            m_velocity.y = 0.0F;
            break;
        }
        case State::MOVING_RIGHT:
        {
            m_velocity.x = VELOCITY;
            m_velocity.y = 0.0F;
            break;
        }
        case State::MOVING_UP_LEFT:
        {
            m_velocity.x = -VELOCITY * DIAGONAL_FACTOR;  // Diagonal movement
            m_velocity.y = -VELOCITY * DIAGONAL_FACTOR;  // Diagonal movement
            break;
        }
        case State::MOVING_UP_RIGHT:
        {
            m_velocity.x = VELOCITY * DIAGONAL_FACTOR;   // Diagonal movement
            m_velocity.y = -VELOCITY * DIAGONAL_FACTOR;  // Diagonal movement
            break;
        }
        case State::MOVING_DOWN_LEFT:
        {
            m_velocity.x = -VELOCITY * DIAGONAL_FACTOR;  // Diagonal movement
            m_velocity.y = VELOCITY * DIAGONAL_FACTOR;   // Diagonal movement
            break;
        }
        case State::MOVING_DOWN_RIGHT:
        {
            m_velocity.x = VELOCITY * DIAGONAL_FACTOR;  // Diagonal movement
            m_velocity.y = VELOCITY * DIAGONAL_FACTOR;  // Diagonal movement
            break;
        }
    }

    m_angle += static_cast<float>(stepMs) * ANGLE_INCREMENT;
    m_position.x += m_velocity.x;                                             // Update position based on velocity
    m_position.y += m_velocity.y;                                             // Update position based on velocity
    m_size = 1.0F + (sinf(m_angle * SCALE_ANGLE_MULTIPLIER) * SCALE_FACTOR);  // Scale between 0.95 and 1.05
}

void PF::Player::handleEvent(PF::PlayerIntention playerIntention)
{
    switch (playerIntention)
    {
        case PF::PlayerIntention::ATTACK:
        {
            m_needToSpawnAttack = true;  // Set the flag to spawn an attack
            break;
        }
        case PF::PlayerIntention::MOVE_UP:
        {
            switch (m_state)
            {
                case State::IDLE: [[fallthrough]];
                case State::MOVING_DOWN:
                {
                    m_state = State::MOVING_UP;
                    break;
                }

                case State::MOVING_LEFT: [[fallthrough]];
                case State::MOVING_DOWN_LEFT:
                {
                    m_state = State::MOVING_UP_LEFT;
                    break;
                }

                case State::MOVING_RIGHT: [[fallthrough]];
                case State::MOVING_DOWN_RIGHT:
                {
                    m_state = State::MOVING_UP_RIGHT;
                    break;
                }
                default: break;
            }
            break;
        }
        case PF::PlayerIntention::MOVE_DOWN:
        {
            switch (m_state)
            {
                case State::IDLE: [[fallthrough]];
                case State::MOVING_UP:
                {
                    m_state = State::MOVING_DOWN;
                    break;
                }

                case State::MOVING_LEFT: [[fallthrough]];
                case State::MOVING_UP_LEFT:
                {
                    m_state = State::MOVING_DOWN_LEFT;
                    break;
                }

                case State::MOVING_RIGHT: [[fallthrough]];
                case State::MOVING_UP_RIGHT:
                {
                    m_state = State::MOVING_DOWN_RIGHT;
                    break;
                }
                default: break;
            }
            break;
        }
        case PF::PlayerIntention::MOVE_LEFT:
        {
            switch (m_state)
            {
                case State::IDLE: [[fallthrough]];
                case State::MOVING_RIGHT:
                {
                    m_state = State::MOVING_LEFT;
                    break;
                }

                case State::MOVING_UP: [[fallthrough]];
                case State::MOVING_UP_RIGHT:
                {
                    m_state = State::MOVING_UP_LEFT;
                    break;
                }

                case State::MOVING_DOWN: [[fallthrough]];
                case State::MOVING_DOWN_RIGHT:
                {
                    m_state = State::MOVING_DOWN_LEFT;
                    break;
                }
                default: break;
            }
            break;
        }
        case PF::PlayerIntention::MOVE_RIGHT:
        {
            switch (m_state)
            {
                case State::IDLE: [[fallthrough]];
                case State::MOVING_LEFT:
                {
                    m_state = State::MOVING_RIGHT;
                    break;
                }

                case State::MOVING_UP: [[fallthrough]];
                case State::MOVING_UP_LEFT:
                {
                    m_state = State::MOVING_UP_RIGHT;
                    break;
                }

                case State::MOVING_DOWN: [[fallthrough]];
                case State::MOVING_DOWN_LEFT:
                {
                    m_state = State::MOVING_DOWN_RIGHT;
                    break;
                }
                default: break;
            }
            break;
        }
        case PF::PlayerIntention::MOVE_STOP_UP:
        {
            switch (m_state)
            {
                case State::MOVING_UP:
                {
                    m_state = State::IDLE;  // Stop moving up
                    break;
                }
                case State::MOVING_UP_LEFT:
                {
                    m_state = State::MOVING_LEFT;  // Stop moving up-left
                    break;
                }
                case State::MOVING_UP_RIGHT:
                {
                    m_state = State::MOVING_RIGHT;  // Stop moving up-right
                    break;
                }
                default: break;
            }
            break;
        }
        case PF::PlayerIntention::MOVE_STOP_DOWN:
        {
            switch (m_state)
            {
                case State::MOVING_DOWN:
                {
                    m_state = State::IDLE;  // Stop moving down
                    break;
                }
                case State::MOVING_DOWN_LEFT:
                {
                    m_state = State::MOVING_LEFT;  // Stop moving down-left
                    break;
                }
                case State::MOVING_DOWN_RIGHT:
                {
                    m_state = State::MOVING_RIGHT;  // Stop moving down-right
                    break;
                }
                default: break;
            }
            break;
        }
        case PF::PlayerIntention::MOVE_STOP_LEFT:
        {
            switch (m_state)
            {
                case State::MOVING_LEFT:
                {
                    m_state = State::IDLE;  // Stop moving left
                    break;
                }
                case State::MOVING_UP_LEFT:
                {
                    m_state = State::MOVING_UP;  // Stop moving up-left
                    break;
                }
                case State::MOVING_DOWN_LEFT:
                {
                    m_state = State::MOVING_DOWN;  // Stop moving down-left
                    break;
                }
                default: break;
            }
            break;
        }
        case PF::PlayerIntention::MOVE_STOP_RIGHT:
        {
            switch (m_state)
            {
                case State::MOVING_RIGHT:
                {
                    m_state = State::IDLE;  // Stop moving right
                    break;
                }
                case State::MOVING_UP_RIGHT:
                {
                    m_state = State::MOVING_UP;  // Stop moving up-right
                    break;
                }
                case State::MOVING_DOWN_RIGHT:
                {
                    m_state = State::MOVING_DOWN;  // Stop moving down-right
                    break;
                }
                default: break;
            }
            break;
        }

        default: break;
    }
}

std::shared_ptr<PF::Object> PF::Player::spawnChildObject()
{
    if (m_needToSpawnAttack)
    {
        m_needToSpawnAttack = false;  // Reset the flag after spawning the attack
        return spawnAttack();         // Spawn an attack object
    }
    return nullptr;  // No child object to spawn
}

std::shared_ptr<PF::Object> PF::Player::spawnAttack() const
{
    const float velocitySum = (m_velocity.x * m_velocity.x) + (m_velocity.y * m_velocity.y);
    SDL_FPoint attackVelocity = m_velocity;
    if (velocitySum < MIN_VELOCITY_THRESHOLD) { attackVelocity = m_lastVelocity; }

    float size = m_size * ATTACK_SIZE_FACTOR;
    auto attack = std::make_shared<PF::Attack>(m_textureIdx, m_srcRect, m_position, size);

    attackVelocity.x *= ATTACK_VELOCITY_MULTIPLIER;
    attackVelocity.y *= ATTACK_VELOCITY_MULTIPLIER;
    attack->setVelocity(attackVelocity);  // Set the velocity of the attack
    return attack;
}

PF::Attack::Attack(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size)
    : Object(textureIdx, srcRect, position, size)
{
}

void PF::Attack::update(Uint64 stepMs)
{
    m_angle += static_cast<float>(stepMs) * ATTACK_ANGLE_INCREMENT;
    float sinAngle = sinf(m_angle);
    float cosAngle = cosf(COS_ANGLE_MULTIPLIER * m_angle);

    m_position.x += m_velocity.x * (1 + sinAngle) + POSITION_OFFSET * sinAngle;  // Update position based on velocity
    m_position.y += m_velocity.y * (1 + cosAngle) + POSITION_OFFSET * cosAngle;  // Update position based on velocity

    // Decelerate x velocity based on proportion of x/y velocity
    if (m_velocity.x > MIN_VELOCITY_THRESHOLD) { m_velocity.x -= ATTACK_DECELERATION; }
    else if (m_velocity.x < -MIN_VELOCITY_THRESHOLD) { m_velocity.x += ATTACK_DECELERATION; }
    else { m_velocity.x = 0.0F; }

    // Decelerate y velocity based on inverse proportion of x/y velocity
    if (m_velocity.y > MIN_VELOCITY_THRESHOLD) { m_velocity.y -= ATTACK_DECELERATION; }
    else if (m_velocity.y < -MIN_VELOCITY_THRESHOLD) { m_velocity.y += ATTACK_DECELERATION; }
    else { m_velocity.y = 0.0F; }

    m_size = m_size + (sinf(m_angle * 3) * ATTACK_SIZE_OSCILLATION) - ATTACK_SIZE_DECAY * m_angle;
}

void PF::Attack::setVelocity(SDL_FPoint velocity) { m_velocity = velocity; }

bool PF::Attack::shouldRemove() const { return m_size < MIN_ATTACK_SIZE; }
