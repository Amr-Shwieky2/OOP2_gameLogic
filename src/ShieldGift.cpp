#include "ShieldGift.h"
#include "Player.h"
#include "PlayerEffectManager.h"

ShieldGift::ShieldGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("ProtectiveShieldGift.png"); 
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.5f, 0.5f);
    m_sprite.setPosition(x, y);
}

void ShieldGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect ShieldGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void ShieldGift::onCollect(Player& player) {
    player.applyEffect(PlayerEffect::Shield, 10.f);  // 10 seconds shield
    collect();
}
