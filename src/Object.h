#pragma once

#include <SDL3/SDL.h>

#include <cstddef>

namespace PF
{
class TextureManager;

class Object
{
  public:
    Object(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size);
    virtual ~Object() = default;

    virtual void update(Uint64 stepMs);
    virtual void handleKeyboardEvent(SDL_Event* event);
    void render(SDL_Renderer* renderer, const PF::TextureManager& textureManager) const;

  private:
    std::size_t m_textureIdx;
    SDL_FRect m_srcRect;

  protected:
    SDL_FPoint m_position;
    float m_size;
};
}  // namespace PF
