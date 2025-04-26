#pragma once

#include <SDL3/SDL.h>

namespace PF::Global
{
namespace Window
{
constexpr int DEFAULT_WIDTH = 1280;
constexpr int DEFAULT_HEIGHT = 720;
extern SDL_Point g_dimensions;
inline SDL_Point GetWindowDimensions() { return g_dimensions; }
inline void SetDimensions(SDL_Point dimensions) { g_dimensions = dimensions; }
}  // namespace Window

namespace Model
{
constexpr int SIMULATION_STEP_RATE_MS = 10;
}  // namespace Model

namespace Colors
{
constexpr float FULL_CHANNEL = 1.0F;
constexpr float EMPTY_CHANNEL = 0.0F;
constexpr SDL_FColor RED = {FULL_CHANNEL, EMPTY_CHANNEL, EMPTY_CHANNEL, SDL_ALPHA_OPAQUE_FLOAT};
constexpr SDL_FColor GREEN = {EMPTY_CHANNEL, FULL_CHANNEL, EMPTY_CHANNEL, SDL_ALPHA_OPAQUE_FLOAT};
constexpr SDL_FColor BLUE = {EMPTY_CHANNEL, EMPTY_CHANNEL, FULL_CHANNEL, SDL_ALPHA_OPAQUE_FLOAT};
constexpr SDL_FColor BLACK = {EMPTY_CHANNEL, EMPTY_CHANNEL, EMPTY_CHANNEL, SDL_ALPHA_OPAQUE_FLOAT};
}  // namespace Colors

}  // namespace PF::Global
