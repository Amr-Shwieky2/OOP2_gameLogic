#include "LifeGift.h"
#include "Player.h"

LifeGift::LifeGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("LifeHeartGift.png"); 
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.5f, 0.5f);
    m_sprite.setPosition(x, y);
}

void LifeGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect LifeGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void LifeGift::onCollect(Player& player) {
    if (player.getLives() < 3) {
        player.addLife(); // you must add this method to Player
        collect();
    }
}
