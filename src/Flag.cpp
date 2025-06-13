#include "Flag.h"

Flag::Flag(float x, float y, TextureManager& textures) {
    sf::Texture& texture = textures.getResource("redFlag.png");
    m_sprite.setTexture(texture);
    m_sprite.setPosition(x, y);
    m_position = { x, y };
}

void Flag::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

bool Flag::isSolid() const {
    return false;
}

sf::FloatRect Flag::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void Flag::update(float) {
    // No logic needed
}
