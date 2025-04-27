#include "Enums.h"

const char* PF::toString(const PF::PlayerIntention playerIntention)
{
    switch (playerIntention)
    {
        case PF::PlayerIntention::NONE: return "NONE";
        case PF::PlayerIntention::MOVE_UP: return "MOVE_UP";
        case PF::PlayerIntention::MOVE_DOWN: return "MOVE_DOWN";
        case PF::PlayerIntention::MOVE_LEFT: return "MOVE_LEFT";
        case PF::PlayerIntention::MOVE_RIGHT: return "MOVE_RIGHT";
        case PF::PlayerIntention::MOVE_STOP_UP: return "MOVE_STOP_UP";
        case PF::PlayerIntention::MOVE_STOP_DOWN: return "MOVE_STOP_DOWN";
        case PF::PlayerIntention::MOVE_STOP_LEFT: return "MOVE_STOP_LEFT";
        case PF::PlayerIntention::MOVE_STOP_RIGHT: return "MOVE_STOP_RIGHT";
        case PF::PlayerIntention::ATTACK: return "ATTACK";
        case PF::PlayerIntention::ATTACK_STOP: return "ATTACK_STOP";
        case PF::PlayerIntention::PlayerIntention_Last: return "UNKNOWN_PLAYER_INTENTION";
    }
    return nullptr;
}
