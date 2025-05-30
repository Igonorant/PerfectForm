#pragma once

#include <SDL3/SDL.h>

#include <cstddef>
#include <memory>

#include "Enums.h"
#include "Object.h"

namespace PF
{
constexpr float DEFAULT_DECELERATION = 0.025F;

class Player : public Object
{
    enum class State
    {
        IDLE,
        MOVING_UP,
        MOVING_DOWN,
        MOVING_LEFT,
        MOVING_RIGHT,
        MOVING_UP_LEFT,
        MOVING_UP_RIGHT,
        MOVING_DOWN_LEFT,
        MOVING_DOWN_RIGHT,
        ATTACKING
    };

  public:
    Player(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size);

    void update(Uint64 stepMs) override;

    void handleEvent(PF::PlayerIntention playerIntention) override;

    [[nodiscard]]
    std::shared_ptr<Object> spawnChildObject() override;

  private:
    [[nodiscard]]
    std::shared_ptr<PF::Object> spawnAttack() const;

    void handleAttackIntention(bool stop);
    void handleMoveUp(bool stop);
    void handleMoveDown(bool stop);
    void handleMoveLeft(bool stop);
    void handleMoveRight(bool stop);

    [[nodiscard]] bool isMovingUp() const;
    [[nodiscard]] bool isMovingDown() const;
    [[nodiscard]] bool isMovingLeft() const;
    [[nodiscard]] bool isMovingRight() const;

  private:
    Uint64 m_playerClock = 0;  // Player clock for timing

    State m_movementState = State::IDLE;  // Current state of the player movement
    State m_actionState = State::IDLE;    // Current state of the player action

    bool m_needToSpawnAttack = false;  // Flag to indicate if an attack should be spawned
    Uint64 m_lastAttackTime = 0;       // Last time the attack was performed

    float m_angle = 0.0F;                      // Angle for circular motion
    SDL_FPoint m_velocity = {0.0F, 0.0F};      // Velocity vector for movement
    SDL_FPoint m_lastVelocity = {0.0F, 0.0F};  // Last velocity vector for movement
};

class Attack : public Object
{
  public:
    Attack(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size);

    void update(Uint64 stepMs) override;

    void render(SDL_Renderer* renderer, const PF::TextureManager& textureManager) const override;

    void setVelocity(SDL_FPoint velocity);

    [[nodiscard]]
    bool shouldRemove() const override;

  private:
    float m_angle = 0.0F;  // Angle for circular motion
    SDL_FPoint m_velocity = {0.0F, 0.0F};
    float m_deceleration = DEFAULT_DECELERATION;  // Deceleration factor for attack movement
};
}  // namespace PF
