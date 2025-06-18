#include "CloseBox.h"

CloseBox::CloseBox(b2World& world, float x, float y, TextureManager& textures) {
    m_sprite.setTexture(textures.getResource("CloseBox.png"));
    m_sprite.setPosition(x, y);
    m_bounds = m_sprite.getGlobalBounds();
}

void CloseBox::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect CloseBox::getBounds() const {
    return m_bounds;
}

void CloseBox::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

bool CloseBox::isOpened() const {
    return m_opened;
}

void CloseBox::open() {
    if (!m_opened) {
        m_opened = true;
        m_sprite.setTextureRect(sf::IntRect(0, 0, 0, 0)); // Or swap to OpenBox texture
        // Optional: play animation or sound
    }
}
