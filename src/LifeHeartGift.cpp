#include "LifeHeartGift.h"

LifeHeartGift::LifeHeartGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("LifeHeartGift.png");
    m_sprite.setTexture(tex);

    m_sprite.setScale(0.2f, 0.2f);
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    m_sprite.setPosition(x, y);
}

void LifeHeartGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect LifeHeartGift::getBounds() const {
    return m_sprite.getGlobalBounds();
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