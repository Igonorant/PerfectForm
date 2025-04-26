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
#include "GlobalDefinitions.h"

namespace
{
[[nodiscard("Exception being handled and returning app failure")]]
SDL_AppResult LogException(const char* msg, SDL_Window* window = nullptr)
{
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", msg);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, window);
    return SDL_APP_FAILURE;
}

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
        while ((now - state->lastStep) >= PF::Global::Model::SIMULATION_STEP_RATE_MS)
        {
            const Uint64 stepMs = now - state->lastStep;
            state->game->update(stepMs);

            // TODO: increment this by any means necessary!
            state->lastStep += PF::Global::Model::SIMULATION_STEP_RATE_MS;
        }

        // Clear the renderer with a color
        if (!SDL_SetRenderDrawColorFloat(state->renderer,
                                         PF::Global::Colors::BLACK.r,
                                         PF::Global::Colors::BLACK.g,
                                         PF::Global::Colors::BLACK.b,
                                         PF::Global::Colors::BLACK.a))
        {
            throw PF::SDLException("Failed to set renderer clear color.");
        }
        if (!SDL_RenderClear(state->renderer)) { throw PF::SDLException("Failed to clear renderer."); }

        state->game->render();  // Render the game objects

        if (!SDL_RenderPresent(state->renderer)) { throw PF::SDLException("Failed to present renderer."); }
    }
    catch (const PF::SDLException& e)
    {
        return LogException(e.what(), state->window);
    }
    catch (const std::exception& e)
    {
        return LogException(e.what());
    }
    return SDL_APP_CONTINUE;
}

namespace
{
void SetAppMetadata()
{
    const std::vector<AppMetadata> extendedMetadata{
        {.key = SDL_PROP_APP_METADATA_CREATOR_STRING, .value = "Igonorant"},
        {   .key = SDL_PROP_APP_METADATA_TYPE_STRING,      .value = "game"}
    };

    if (!SDL_SetAppMetadata("Perfect Form", "0.1", "com.igonorant.perfectform"))
    {
        throw PF::SDLException("Failed to set app metadata.");
    }

    for (const auto& [key, value] : extendedMetadata)
    {
        if (!SDL_SetAppMetadataProperty(key, value))
        {
            throw PF::SDLException(std::format("Failed to set app metadata property: {} = {}", key, value));
        }
    }
}

void InitializeSDL()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) { throw PF::SDLException("Couldn't initialize SDL."); }
}

void InitializeWindowAndRenderer(std::unique_ptr<AppState>& appState)
{
    if (!SDL_CreateWindowAndRenderer("Perfect Form",
                                     PF::Global::Window::DEFAULT_WIDTH,
                                     PF::Global::Window::DEFAULT_HEIGHT,
                                     0,
                                     &appState->window,
                                     &appState->renderer))
    {
        throw PF::SDLException("Failed to create window and renderer.");
    }

    // Ensuring the global variables are updated to the actual window size.
    int width = PF::Global::Window::DEFAULT_WIDTH;
    int height = PF::Global::Window::DEFAULT_HEIGHT;
    SDL_GetWindowSize(appState->window, &width, &height);
    PF::Global::Window::SetDimensions({width, height});
}
}  // namespace

SDL_AppResult SDL_AppInit(void** appState, int /*argc*/, char* /*argv*/[])
{
    try
    {
        SetAppMetadata();
        InitializeSDL();

        g_appState = std::make_unique<AppState>();
        if (g_appState == nullptr) { throw PF::SDLException("Failed to allocate memory for AppState."); }

        InitializeWindowAndRenderer(g_appState);

        g_appState->game = std::make_unique<PF::Game>(g_appState->renderer);
        g_appState->lastStep = SDL_GetTicks();
        *appState = g_appState.get();
        SDL_Log("Application initialized successfully.");
    }
    catch (const PF::SDLException& e)
    {
        return LogException(e.what(),
                            nullptr);  // window may not be available yet, so using nullptr instead (no parent)
    }
    catch (const std::exception& e)
    {
        return LogException(e.what());
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
            default:
            {
                state->game->handleEvent(event);
                break;
            }
        }
    }
    catch (const PF::SDLException& e)
    {
        return LogException(e.what(), state->window);
    }
    catch (const std::exception& e)
    {
        return LogException(e.what());
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appState, SDL_AppResult /*result*/)
{
    if (appState != nullptr)
    {
        auto* state = static_cast<AppState*>(appState);
        SDL_DestroyRenderer(state->renderer);
        SDL_DestroyWindow(state->window);
    }
    SDL_Log("Application quit successfully.");
}
