#include "ProtectiveShieldGift.h"
#include "Player.h"

ProtectiveShieldGift::ProtectiveShieldGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("ProtectiveShieldGift.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.5f, 0.5f);
    m_sprite.setPosition(x, y);
}

void ProtectiveShieldGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect ProtectiveShieldGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}



void ProtectiveShieldGift::onCollect(Player& player) {
    if (!m_collected) {
        m_collected = true;
        player.applyEffect(PlayerEffect::Shield, 2.f);
    }
}

bool ProtectiveShieldGift::isCollected() const {
    return m_collected;
}
