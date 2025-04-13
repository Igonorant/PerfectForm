#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SIMULATION_STEP_RATE_IN_MILLISECONDS 7
#define SDL_WINDOW_WIDTH 1280
#define SDL_WINDOW_HEIGHT 720

static SDL_Joystick *joystick = NULL;

struct AppState
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    Uint64 lastStep;
};

SDL_AppResult SDL_AppIterate(void *as)
{
    AppState *appState = (AppState *)as;
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

    SDL_SetRenderDrawColor(appState->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(appState->renderer);
    // ADD RENDERING LOGIC HERE
    SDL_RenderPresent(appState->renderer);
    return SDL_APP_CONTINUE;
}

static const struct
{
    const char *key;
    const char *value;
} extended_metadata[] = {
    {SDL_PROP_APP_METADATA_CREATOR_STRING, "Igonorant"},
    {   SDL_PROP_APP_METADATA_TYPE_STRING,      "game"}
};

SDL_AppResult SDL_AppInit(void **as, int /*argc*/, char * /*argv*/[])
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

    AppState *appState = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (!appState) { return SDL_APP_FAILURE; }
    *as = appState;

    if (!SDL_CreateWindowAndRenderer("Perfect Form", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, 0, &appState->window,
                                     &appState->renderer))
    {
        return SDL_APP_FAILURE;
    }

    // INITIALIZE GAME STUFF HERE

    appState->lastStep = SDL_GetTicks();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void * /*as*/, SDL_Event *event)
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

void SDL_AppQuit(void *as, SDL_AppResult /*result*/)
{
    if (joystick) { SDL_CloseJoystick(joystick); }
    if (as != NULL)
    {
        AppState *appState = (AppState *)as;
        SDL_DestroyRenderer(appState->renderer);
        SDL_DestroyWindow(appState->window);
        SDL_free(appState);
    }
}
