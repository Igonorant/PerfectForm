#include <memory>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <exception>
#include <format>
#include <vector>

#include "Exceptions.h"
#include "Game.h"

#define SIMULATION_STEP_RATE_IN_MILLISECONDS 10
#define SDL_WINDOW_WIDTH 1280
#define SDL_WINDOW_HEIGHT 720

#define DEFAULT_LOG_EXCEPTION(msg)                                         \
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", msg);                       \
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, nullptr); \
    return SDL_APP_FAILURE;

#define SDL_LOG_EXCEPTION(msg, window)                                    \
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", msg);                      \
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, window); \
    return SDL_APP_FAILURE;

namespace
{
SDL_Joystick* g_joystick = nullptr;

struct AppState
{
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};

    Uint64 lastStep{0};

    std::unique_ptr<PF::Game> game{nullptr};
};

std::unique_ptr<AppState> g_appState{nullptr};

struct AppMetadata
{
    const char* key;
    const char* value;
};

}  // namespace

SDL_AppResult SDL_AppIterate(void* appState)
{
    auto* state = static_cast<AppState*>(appState);
    try
    {
        const Uint64 now = SDL_GetTicks();

        // FIXME: this can become unsafe very easily, we need to ensure lastStep is always less than now.
        // run game logic if we're at or past the time to run it.
        // if we're _really_ behind the time to run it, run it
        // several times.
        while ((now - state->lastStep) >= SIMULATION_STEP_RATE_IN_MILLISECONDS)
        {
            const Uint64 stepMs = now - state->lastStep;
            state->game->update(stepMs);

            // TODO: increment this by any means necessary!
            state->lastStep += SIMULATION_STEP_RATE_IN_MILLISECONDS;
        }

        // Clear the renderer with a color
        const Uint8 clearR = 255;
        const Uint8 clearG = 230;
        const Uint8 clearB = 190;
        SDL_Color clearColor = {clearR, clearG, clearB, SDL_ALPHA_OPAQUE};
        if (!SDL_SetRenderDrawColor(state->renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a))
        {
            throw PF::SDLException("Failed to set renderer clear color.");
        }
        if (!SDL_RenderClear(state->renderer)) { throw PF::SDLException("Failed to clear renderer."); }

        state->game->render();  // Render the game objects

        if (!SDL_RenderPresent(state->renderer)) { throw PF::SDLException("Failed to present renderer."); }
    }
    catch (const PF::SDLException& e)
    {
        SDL_LOG_EXCEPTION(e.what(), state->window);
    }
    catch (const std::exception& e)
    {
        DEFAULT_LOG_EXCEPTION(e.what());
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void** appState, int /*argc*/, char* /*argv*/[])
{
    try
    {
        if (!SDL_SetAppMetadata("Perfect Form", "0.1", "com.igonorant.perfectform"))
        {
            throw PF::SDLException("Failed to set app metadata.");
        }
        const std::vector<AppMetadata> extendedMetadata{
            {.key = SDL_PROP_APP_METADATA_CREATOR_STRING, .value = "Igonorant"},
            {   .key = SDL_PROP_APP_METADATA_TYPE_STRING,      .value = "game"}
        };
        for (const auto& [key, value] : extendedMetadata)
        {
            if (!SDL_SetAppMetadataProperty(key, value))
            {
                throw PF::SDLException(std::format("Failed to set app metadata property: {} = {}", key, value));
            }
        }

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) { throw PF::SDLException("Couldn't initialize SDL."); }

        g_appState = std::make_unique<AppState>();
        if (g_appState == nullptr) { throw PF::SDLException("Failed to allocate memory for AppState."); }
        // auto* state = static_cast<AppState*>(SDL_calloc(1, sizeof(AppState)));
        if (g_appState == nullptr) { throw PF::SDLException("Failed to allocate memory for AppState."); }

        if (!SDL_CreateWindowAndRenderer("Perfect Form", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, 0, &g_appState->window,
                                         &g_appState->renderer))
        {
            throw PF::SDLException("Failed to create window and renderer.");
        }

        g_appState->game = std::make_unique<PF::Game>(g_appState->renderer);
        g_appState->lastStep = SDL_GetTicks();
        *appState = g_appState.get();
        SDL_Log("Application initialized successfully.");
    }
    catch (const PF::SDLException& e)
    {
        SDL_LOG_EXCEPTION(e.what(), nullptr);  // window may not be available yet, so using nullptr instead (no parent)
    }
    catch (const std::exception& e)
    {
        DEFAULT_LOG_EXCEPTION(e.what());
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event)
{
    auto* state = static_cast<AppState*>(appState);
    try
    {
        switch (event->type)
        {
            case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
            case SDL_EVENT_JOYSTICK_ADDED:
                if (g_joystick == nullptr)
                {
                    g_joystick = SDL_OpenJoystick(event->jdevice.which);
                    if (g_joystick == nullptr)
                    {
                        throw PF::SDLException(std::format("Failed to open joystick ID {}:", event->jdevice.which));
                    }
                }
                break;
            case SDL_EVENT_JOYSTICK_REMOVED:
                if ((g_joystick != nullptr) && (SDL_GetJoystickID(g_joystick) == event->jdevice.which))
                {
                    SDL_CloseJoystick(g_joystick);
                    g_joystick = nullptr;
                }
                break;
            case SDL_EVENT_JOYSTICK_HAT_MOTION:
                // HANDLE JOYSTICK HAT MOTION HERE
            case SDL_EVENT_KEY_DOWN: [[fallthrough]];
            case SDL_EVENT_KEY_UP:
            {
                state->game->handleKeyboardEvent(event);
                break;
            }
            default: break;
        }
    }
    catch (const PF::SDLException& e)
    {
        SDL_LOG_EXCEPTION(e.what(), state->window);
    }
    catch (const std::exception& e)
    {
        DEFAULT_LOG_EXCEPTION(e.what());
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appState, SDL_AppResult /*result*/)
{
    if (g_joystick != nullptr) { SDL_CloseJoystick(g_joystick); }
    if (appState != nullptr)
    {
        auto* state = static_cast<AppState*>(appState);
        SDL_DestroyRenderer(state->renderer);
        SDL_DestroyWindow(state->window);
    }
    SDL_Log("Application quit successfully.");
}
