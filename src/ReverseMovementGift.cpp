#include "ReverseMovementGift.h"
#include "GameObjectVisitor.h"

ReverseMovementGift::ReverseMovementGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("ReverseMovementGift.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.08f, 0.08f);
    m_sprite.setPosition(x, y);
}

void ReverseMovementGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect ReverseMovementGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void ReverseMovementGift::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

void ReverseMovementGift::collect() {
    m_collected = true;
}

bool ReverseMovementGift::isCollected() const {
    return m_collected;
}
