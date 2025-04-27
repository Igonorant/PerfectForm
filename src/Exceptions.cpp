#include <SDL3/SDL.h>

#include <format>
#include <stdexcept>
#include <string>
#include <string_view>

#include "Exceptions.h"


PF::Exception::Exception(std::string_view message): std::runtime_error(std::string(message)) {}

PF::SDLException::SDLException(std::string_view message)
    : std::runtime_error(std::format("SDL Exception: {}\n  {}", message, SDL_GetError()))
{
}
