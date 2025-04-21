#include <SDL3/SDL.h>

#include <stdexcept>
#include <string_view>

namespace PF
{

class SDLException : public std::runtime_error
{
  public:
    explicit SDLException(std::string_view message);
};

}  // namespace PF
