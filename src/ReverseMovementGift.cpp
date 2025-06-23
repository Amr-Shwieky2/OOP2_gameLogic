#include "ReverseMovementGift.h"
#include "Player.h"

ReverseMovementGift::ReverseMovementGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("ReverseMovementGift.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.5f, 0.5f);
    m_sprite.setPosition(x, y);
}

void ReverseMovementGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect ReverseMovementGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}



void ReverseMovementGift::onCollect(Player& player) {
    if (!m_collected) {
        m_collected = true;
        player.applyEffect(PlayerEffect::ReverseControl, 3.f);
    }
}

bool ReverseMovementGift::isCollected() const {
    return m_collected;
}
