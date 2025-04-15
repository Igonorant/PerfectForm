#include <cassert>
#include <cstddef>

#include "Object.h"
#include "TextureManager.h"

PF::Object::Object(std::size_t textureIdx, SDL_FRect srcRect, SDL_FPoint position, float size)
    : m_textureIdx(textureIdx), m_srcRect(srcRect), m_position(position), m_size(size)
{
}

void PF::Object::render(SDL_Renderer* renderer, const PF::TextureManager& textureManager) const
{
    auto& texture = textureManager.getTexture(m_textureIdx).get();
    const auto width = m_srcRect.w * m_size;
    const auto height = m_srcRect.h * m_size;
    SDL_FRect dstRect = {m_position.x - width / 2, m_position.y - height / 2, width, height};
    const bool success = SDL_RenderTexture(renderer, &texture, &m_srcRect, &dstRect);
    assert(success);
}

void PF::Object::update(Uint64 stepMs)
{
    // Default implementation does nothing. Derived classes can override this method to provide specific behavior.
    // For example, you might want to update the position or state of the object based on the elapsed time (stepMs).
    assert(0 && "update() method should be overridden in derived classes.");
}
