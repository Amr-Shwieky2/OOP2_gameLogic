// SpeedGift.cpp
#include "SpeedGift.h"

SpeedGift::SpeedGift(float x, float y, TextureManager& textures) {
    m_sprite.setTexture(textures.getResource("SpeedGift.png"));
    m_sprite.setPosition(x, y);
    m_sprite.setScale(0.5f, 0.5f);
}

void SpeedGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect SpeedGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}



void SpeedGift::onCollect(Player& player) {
    if (!m_collected) {
        m_collected = true;
        player.applyEffect(PlayerEffect::SpeedBoost, 1);
    }
}

bool SpeedGift::isCollected() const {
    return m_collected;
}
