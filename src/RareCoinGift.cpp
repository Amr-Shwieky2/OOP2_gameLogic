#include "RareCoinGift.h"
#include "Player.h"

RareCoinGift::RareCoinGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("RareCoinGift.png");
    m_sprite.setTexture(tex);

    m_sprite.setScale(0.2f, 0.2f);
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    m_sprite.setPosition(x, y);
}


void RareCoinGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect RareCoinGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}



void RareCoinGift::onCollect(Player& player) {
    if (!m_collected) {
        m_collected = true;
        player.increaseScore(30);  
    }
}

bool RareCoinGift::isCollected() const {
    return m_collected;
}