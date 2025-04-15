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

    void render(SDL_Renderer* renderer, const PF::TextureManager& textureManager) const;
    virtual void update(Uint64 stepMs);

  private:
    std::size_t m_textureIdx;
    SDL_FRect m_srcRect;

  protected:
    SDL_FPoint m_position;
    float m_size;
};
}  // namespace PF
