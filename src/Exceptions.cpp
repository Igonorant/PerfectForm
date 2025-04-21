#include <format>
#include <stdexcept>
#include <string_view>

#include "Exceptions.h"


PF::SDLException::SDLException(std::string_view message)
    : std::runtime_error(std::format("SDL Exception: {}\n  {}", message, SDL_GetError()))
{
}
