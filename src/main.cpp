#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>

#include <limits>
#include <stdexcept>

#include "Game.h"

#define SIMULATION_STEP_RATE_IN_MILLISECONDS 7
#define SDL_WINDOW_WIDTH 1280
#define SDL_WINDOW_HEIGHT 720

static SDL_Joystick* joystick = NULL;

struct AppState
{
    SDL_Window* window;
    SDL_Renderer* renderer;

    Uint64 lastStep;

    PF::Game game;
    std::size_t textureIndex = std::numeric_limits<std::size_t>::max();
};

SDL_AppResult SDL_AppIterate(void* as)
{
    AppState* appState = (AppState*)as;
    const Uint64 now = SDL_GetTicks();

    // run game logic if we're at or past the time to run it.
    // if we're _really_ behind the time to run it, run it
    // several times.

    while ((now - appState->lastStep) >= SIMULATION_STEP_RATE_IN_MILLISECONDS)
    {
        // DO GAME LOGIC HERE

        // TODO: increment this by any means necessary!
        appState->lastStep += SIMULATION_STEP_RATE_IN_MILLISECONDS;
    }

    SDL_SetRenderDrawColor(appState->renderer, 255, 230, 190, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(appState->renderer);
    // ADD RENDERING LOGIC HERE

    float angle = (float)(now * 0.0007f);  // Convert ms to seconds for smoother motion
    float orbitX = sinf(angle) * 100.0f;   // Oscillates between -100.0f and 100.0f
    float orbitY = cosf(angle) * 100.0f;   // Creates circular pattern when combined with scale_x

    auto& texture = appState->game.getTextureManager().getTexture(appState->textureIndex).get();

    SDL_FRect dst_rect;
    dst_rect.x = orbitX + SDL_WINDOW_WIDTH / 2.0f - texture.w / 2.0f;
    dst_rect.y = orbitY + SDL_WINDOW_HEIGHT / 2.0f - texture.h / 2.0f;
    dst_rect.w = texture.w * 2.0f;
    dst_rect.h = texture.h * 2.0f;

    // SDL_RenderTexture(appState->renderer, &texture, NULL, &dst_rect);
    double rotationAngle = now * 0.1f;  // Rotate 0.1 degrees per millisecond
    SDL_RenderTextureRotated(appState->renderer, &texture, NULL, &dst_rect, rotationAngle, NULL, SDL_FLIP_NONE);

    SDL_RenderPresent(appState->renderer);
    return SDL_APP_CONTINUE;
}

static const struct
{
    const char* key;
    const char* value;
} extended_metadata[] = {
    {SDL_PROP_APP_METADATA_CREATOR_STRING, "Igonorant"},
    {   SDL_PROP_APP_METADATA_TYPE_STRING,      "game"}
};

SDL_AppResult SDL_AppInit(void** as, int /*argc*/, char* /*argv*/[])
{
    size_t i;

    if (!SDL_SetAppMetadata("Perfect Form", "0.1", "com.igonorant.perfectform")) { return SDL_APP_FAILURE; }
    for (i = 0; i < SDL_arraysize(extended_metadata); i++)
    {
        if (!SDL_SetAppMetadataProperty(extended_metadata[i].key, extended_metadata[i].value))
        {
            return SDL_APP_FAILURE;
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    AppState* appState = (AppState*)SDL_calloc(1, sizeof(AppState));
    if (!appState) { return SDL_APP_FAILURE; }
    if (!SDL_CreateWindowAndRenderer("Perfect Form", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, 0, &appState->window,
                                     &appState->renderer))
    {
        return SDL_APP_FAILURE;
    }
    appState->game = PF::Game(appState->renderer);
    *as = appState;

    try
    {
        constexpr std::string_view cellAsset = "../../assets/BaseCell_32x32.png";
        appState->textureIndex = appState->game.getTextureManager().addTexture(cellAsset);
    }
    catch (const std::exception& e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, e.what());
        return SDL_APP_FAILURE;
    }

    appState->lastStep = SDL_GetTicks();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* /*as*/, SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
        case SDL_EVENT_JOYSTICK_ADDED:
            if (joystick == NULL)
            {
                joystick = SDL_OpenJoystick(event->jdevice.which);
                if (!joystick)
                {
                    SDL_Log("Failed to open joystick ID %u: %s", (unsigned int)event->jdevice.which, SDL_GetError());
                }
            }
            break;
        case SDL_EVENT_JOYSTICK_REMOVED:
            if (joystick && (SDL_GetJoystickID(joystick) == event->jdevice.which))
            {
                SDL_CloseJoystick(joystick);
                joystick = NULL;
            }
            break;
        case SDL_EVENT_JOYSTICK_HAT_MOTION:
            // HANDLE JOYSTICK HAT MOTION HERE
        case SDL_EVENT_KEY_DOWN:
            // HANDLE KEYBOARD INPUT HERE
        default: break;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* as, SDL_AppResult /*result*/)
{
    if (joystick) { SDL_CloseJoystick(joystick); }
    if (as != NULL)
    {
        AppState* appState = (AppState*)as;
        SDL_DestroyRenderer(appState->renderer);
        SDL_DestroyWindow(appState->window);
        SDL_free(appState);
    }
}
