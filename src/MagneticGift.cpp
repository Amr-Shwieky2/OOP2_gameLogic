#include "MagneticGift.h"

MagneticGift::MagneticGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("MagneticGift.png");
    m_sprite.setTexture(tex);

    m_sprite.setScale(0.2f, 0.2f);
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    m_sprite.setPosition(x, y);
}

void MagneticGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect MagneticGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void MagneticGift::onCollect(Player& player) {
    if (!m_collected) {
        m_collected = true;
        player.applyEffect(PlayerEffect::Magnetic, 6.f);
    }
}

bool MagneticGift::isCollected() const {
    return m_collected;
}