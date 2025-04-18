#pragma once

#include <SDL3/SDL.h>

#include <cstddef>

#include "Object.h"

namespace PF
{
constexpr float DEFAULT_DECELERATION = 0.025F;

class Player : public Object
{
  public:
    Player(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size);

    void update(Uint64 stepMs) override;

    std::shared_ptr<PF::Object> handleKeyboardEvent(SDL_Event* event) override;

  private:
    std::shared_ptr<PF::Object> spawnAttack() const;

  private:
    float m_angle = 0.0f;                  // Angle for circular motion
    SDL_FPoint m_velocity = {0.0f, 0.0f};  // Velocity vector for movement
};

class Attack : public Object
{
  public:
    Attack(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size);

    void update(Uint64 stepMs) override;

    void setVelocity(SDL_FPoint velocity);

    bool shouldRemove() const override;

  private:
    float m_angle = 0.0f;  // Angle for circular motion
    SDL_FPoint m_velocity = {0.0f, 0.0f};
    float m_deceleration = DEFAULT_DECELERATION;  // Deceleration factor for attack movement
};
}  // namespace PF
