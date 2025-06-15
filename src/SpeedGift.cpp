#include "SpeedGift.h"
#include "Player.h"
#include "PlayerEffectManager.h"

SpeedGift::SpeedGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("SpeedGift.png"); 
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.5f, 0.5f);
    m_sprite.setPosition(x, y);
}

void SpeedGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect SpeedGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void SpeedGift::onCollect(Player& player) {
    player.applyEffect(PlayerEffect::SpeedBoost, 5.f); // 5 seconds
    collect();
}
