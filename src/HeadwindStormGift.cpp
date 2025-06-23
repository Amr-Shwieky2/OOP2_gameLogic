#include "HeadwindStormGift.h"
#include "Player.h"

HeadwindStormGift::HeadwindStormGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("HeadwindStormGift.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.5f, 0.5f);
    m_sprite.setPosition(x, y);
}

void HeadwindStormGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect HeadwindStormGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}



void HeadwindStormGift::onCollect(Player& player) {
    if (!m_collected) {
        m_collected = true;
        player.applyEffect(PlayerEffect::Headwind, 4.f);
    }
}

bool HeadwindStormGift::isCollected() const {
    return m_collected;
}
