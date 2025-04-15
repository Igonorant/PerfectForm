#include <math.h>

#include "Player.h"

PF::Player::Player(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size)
    : Object(textureIdx, srcRect, position, size)
{
}

void PF::Player::update(Uint64 stepMs)
{
    m_angle = (float)(stepMs * 0.0007f) + m_angle;  // Convert ms to seconds for smoother motion
    float orbitX = sinf(m_angle) * 100.0f;          // Oscillates between -100.0f and 100.0f
    float orbitY = cosf(m_angle) * 100.0f;          // Creates circular pattern when combined with scale_x

    m_position.x = orbitX + 1280 / 2.0f;
    m_position.y = orbitY + 720 / 2.0f;
}
