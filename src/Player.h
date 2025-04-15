#pragma once

#include <SDL3/SDL.h>

#include <cstddef>

#include "Object.h"

namespace PF
{
class Player : public Object
{
  public:
    Player(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size);

    void update(Uint64 stepMs) override;

  private:
    float m_angle = 0.0f;  // Angle for circular motion
};
}  // namespace PF
