// RareCoinGift.cpp (محدث)
#include "RareCoinGift.h"
#include <iostream>
#include <cmath>

RareCoinGift::RareCoinGift(float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    try {
        m_sprite.setTexture(textures.getResource("RareCoinGift.png"));
        m_sprite.setPosition(x, y);
        m_sprite.setScale(0.5f, 0.5f);

        // Set origin to center for better effects
        sf::Vector2u textureSize = m_sprite.getTexture()->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    }
    catch (const std::exception& e) {
        // Fallback to regular coin texture if rare coin texture not found
        try {
            m_sprite.setTexture(textures.getResource("coin.png"));
            m_sprite.setPosition(x, y);
            m_sprite.setScale(0.1f, 0.1f); // Slightly larger than regular coin
        }
        catch (const std::exception& e2) {
            std::cerr << "Error creating RareCoinGift: " << e2.what() << std::endl;
        }
    }
}

void RareCoinGift::render(sf::RenderTarget& target) const {
    if (!m_collected) {
        // Add glowing effect for rare coin
        sf::Sprite glowSprite = m_sprite;

        // Simple glow effect using color modulation
        static float glowTimer = 0.0f;
        glowTimer += 0.016f; // Approximate 60 FPS

        float glow = 0.7f + 0.3f * sin(glowTimer * 4.0f); // Pulsing between 0.7 and 1.0
        sf::Uint8 glowValue = static_cast<sf::Uint8>(255 * glow);

        glowSprite.setColor(sf::Color(255, 255, glowValue, 200)); // Golden glow
        target.draw(glowSprite);

        // Draw main sprite
        target.draw(m_sprite);
    }
}

sf::FloatRect RareCoinGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void RareCoinGift::onCollect(GameState& gameState) {
    if (!m_collected) {
        m_collected = true;

        //  إضافة النقاط الكبيرة للعملة النادرة
        gameState.addScore(getValue());
    }
}
