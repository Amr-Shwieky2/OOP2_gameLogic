// LifeHeartGift.cpp
#include "LifeHeartGift.h"

LifeHeartGift::LifeHeartGift(float x, float y, TextureManager& textures) {
    m_sprite.setTexture(textures.getResource("LifeHeart.png"));
    m_sprite.setPosition(x, y);
    m_sprite.setScale(0.5f, 0.5f);
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

void LifeHeartGift::onCollect(Player& player) {
    if (!m_collected) {
        m_collected = true;
        player.addLife();
    }
}

bool LifeHeartGift::isCollected() const {
    return m_collected;
}
