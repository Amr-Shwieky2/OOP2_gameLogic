#include "Flag.h"

Flag::Flag(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("redflag.png");
    m_sprite.setTexture(tex);
    m_sprite.setPosition(x, y);
    m_bounds = m_sprite.getGlobalBounds();
}

void Flag::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect Flag::getBounds() const {
    return m_bounds;
}

