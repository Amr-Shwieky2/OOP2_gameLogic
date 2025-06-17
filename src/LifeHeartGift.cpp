#include "LifeHeartGift.h"
#include "GameObjectVisitor.h"

LifeHeartGift::LifeHeartGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("LifeHeartGift.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.08f, 0.08f);
    m_sprite.setPosition(x, y);
}

void LifeHeartGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect LifeHeartGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void LifeHeartGift::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

void LifeHeartGift::collect() {
    m_collected = true;
}

bool LifeHeartGift::isCollected() const {
    return m_collected;
}
