#include <stdexcept>
#include <string_view>

namespace PF
{

class Exception : public std::runtime_error
{
  public:
    explicit Exception(std::string_view message);
};

class SDLException : public std::runtime_error
{
  public:
    explicit SDLException(std::string_view message);
};

}  // namespace PF
