#include "SpeedGift.h"
#include <iostream>

SpeedGift::SpeedGift(float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    try {
        m_sprite.setTexture(textures.getResource("SpeedGift.png"));
        m_sprite.setPosition(x, y);
        m_sprite.setScale(0.5f, 0.5f);

        // Set origin to center
        sf::Vector2u textureSize = m_sprite.getTexture()->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    }
    catch (const std::exception& e) {
        std::cerr << " Error creating SpeedGift: " << e.what() << std::endl;
    }
}

void SpeedGift::render(sf::RenderTarget& target) const {
    if (!m_collected) {
        target.draw(m_sprite);
    }
}

sf::FloatRect SpeedGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void SpeedGift::onCollect(GameState& gameState) {
    if (!m_collected) {
        m_collected = true;
        gameState.addScore(getValue());
    }
}