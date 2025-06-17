#include "ProtectiveShieldGift.h"
#include "GameObjectVisitor.h"

ProtectiveShieldGift::ProtectiveShieldGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("ProtectiveShieldGift.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.08f, 0.08f);
    m_sprite.setPosition(x, y);
}

void ProtectiveShieldGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect ProtectiveShieldGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void ProtectiveShieldGift::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

void ProtectiveShieldGift::collect() {
    m_collected = true;
}

bool ProtectiveShieldGift::isCollected() const {
    return m_collected;
}
