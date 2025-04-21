#include <format>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#include <SDL3/SDL.h>  // IWYU pragma: export
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "Exceptions.h"
#include "Game.h"

#define SIMULATION_STEP_RATE_IN_MILLISECONDS 7
#define SDL_WINDOW_WIDTH 1280
#define SDL_WINDOW_HEIGHT 720

#define SDL_LOG_EXCEPTION(msg, window)                                    \
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", msg);                      \
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, window); \
    return SDL_APP_FAILURE;

static SDL_Joystick* joystick = NULL;

struct AppState
{
    SDL_Window* window;
    SDL_Renderer* renderer;

    Uint64 lastStep;

    PF::Game game;
};

SDL_AppResult SDL_AppIterate(void* as)
{
    AppState* appState = (AppState*)as;
    try
    {
        const Uint64 now = SDL_GetTicks();

        // run game logic if we're at or past the time to run it.
        // if we're _really_ behind the time to run it, run it
        // several times.

        while ((now - appState->lastStep) >= SIMULATION_STEP_RATE_IN_MILLISECONDS)
        {
            const Uint64 stepMs = now - appState->lastStep;
            appState->game.update(stepMs);

            // TODO: increment this by any means necessary!
            appState->lastStep += SIMULATION_STEP_RATE_IN_MILLISECONDS;
        }

        // Clear the renderer with a color
        SDL_Color clearColor = {255, 230, 190, SDL_ALPHA_OPAQUE};
        if (!SDL_SetRenderDrawColor(appState->renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a))
        {
            throw PF::SDLException("Failed to set renderer clear color.");
        }
        if (!SDL_RenderClear(appState->renderer)) { throw PF::SDLException("Failed to clear renderer."); }

        appState->game.render();  // Render the game objects

        if (!SDL_RenderPresent(appState->renderer)) { throw PF::SDLException("Failed to present renderer."); }
    }
    catch (const PF::SDLException& e)
    {
        SDL_LOG_EXCEPTION(e.what(), appState->window);
    }
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
    try
    {
        if (!SDL_SetAppMetadata("Perfect Form", "0.1", "com.igonorant.perfectform"))
        {
            throw PF::SDLException("Failed to set app metadata.");
        }
        for (const auto& [key, value] : extended_metadata)
        {
            if (!SDL_SetAppMetadataProperty(key, value))
            {
                throw PF::SDLException(std::format("Failed to set app metadata property: {} = {}", key, value));
            }
        }

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) { throw PF::SDLException("Couldn't initialize SDL."); }

        AppState* appState = (AppState*)SDL_calloc(1, sizeof(AppState));
        if (!appState) { throw PF::SDLException("Failed to allocate memory for AppState."); }

        if (!SDL_CreateWindowAndRenderer("Perfect Form", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, 0, &appState->window,
                                         &appState->renderer))
        {
            throw PF::SDLException("Failed to create window and renderer.");
        }

        appState->game = PF::Game(appState->renderer);
        *as = appState;

        appState->lastStep = SDL_GetTicks();
    }
    catch (const PF::SDLException& e)
    {
        SDL_LOG_EXCEPTION(e.what(), nullptr);  // window may not be available yet, so using nullptr instead (no parent)
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* as, SDL_Event* event)
{
    AppState* appState = (AppState*)as;
    try
    {
        switch (event->type)
        {
            case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
            case SDL_EVENT_JOYSTICK_ADDED:
                if (joystick == nullptr)
                {
                    joystick = SDL_OpenJoystick(event->jdevice.which);
                    if (!joystick)
                    {
                        throw PF::SDLException(std::format("Failed to open joystick ID {}:", event->jdevice.which));
                    }
                }
                break;
            case SDL_EVENT_JOYSTICK_REMOVED:
                if ((joystick != nullptr) && (SDL_GetJoystickID(joystick) == event->jdevice.which))
                {
                    SDL_CloseJoystick(joystick);
                    joystick = nullptr;
                }
                break;
            case SDL_EVENT_JOYSTICK_HAT_MOTION:
                // HANDLE JOYSTICK HAT MOTION HERE
            case SDL_EVENT_KEY_DOWN: [[fallthrough]];
            case SDL_EVENT_KEY_UP:
            {
                appState->game.handleKeyboardEvent(event);
                break;
            }
            default: break;
        }
    }
    catch (const PF::SDLException& e)
    {
        SDL_LOG_EXCEPTION(e.what(), appState->window);
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
