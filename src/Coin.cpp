#include "Coin.h"
#include "Player.h"

Coin::Coin(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("coin.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.5f, 0.5f);
    m_sprite.setPosition(x, y);
}

void Coin::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect Coin::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void Coin::onCollect(Player& player) {
    player.addScore(10);
    collect();
}
