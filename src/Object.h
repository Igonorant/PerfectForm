#pragma once

#include <SDL3/SDL.h>

#include <cstddef>
#include <memory>

namespace PF
{
class TextureManager;

class Object
{
  public:
    Object(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size);
    Object(const Object&) = default;
    Object(Object&&) = default;
    Object& operator=(const Object&) = default;
    Object& operator=(Object&&) = default;
    virtual ~Object() = default;

    virtual void update(Uint64 stepMs);
    virtual std::shared_ptr<Object> handleKeyboardEvent(SDL_Event* event);
    virtual bool shouldRemove() const;

    void render(SDL_Renderer* renderer, const PF::TextureManager& textureManager) const;

  protected:
    std::size_t m_textureIdx;
    SDL_FRect m_srcRect;
    SDL_FPoint m_position;
    float m_size;
};
}  // namespace PF
