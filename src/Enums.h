#pragma once

namespace PF
{
enum class PlayerIntention
{
    NONE,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_STOP_UP,
    MOVE_STOP_DOWN,
    MOVE_STOP_LEFT,
    MOVE_STOP_RIGHT,
    ATTACK,
    ATTACK_STOP,
    PlayerIntention_Last
};

[[nodiscard]]
const char* toString(PlayerIntention playerIntention);
}  // namespace PF
