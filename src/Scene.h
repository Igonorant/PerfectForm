#include <SDL3/SDL.h>

#include <memory>
#include <utility>

namespace PF
{
class TextureManager;
class Scene
{
  public:
    explicit Scene(std::shared_ptr<const PF::TextureManager> textureManager)
        : m_textureManager(std::move(textureManager)) {};
    virtual ~Scene() = default;

    // Deleted copy and move constructors and assignment operators
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    virtual void update(Uint64 stepMs) = 0;

    // TODO: update this to return the next scene enum when needed
    virtual void handleEvent(SDL_Event* event) = 0;

    virtual void render() const = 0;

  protected:
    const std::shared_ptr<const PF::TextureManager> m_textureManager;
};
}  // namespace PF
