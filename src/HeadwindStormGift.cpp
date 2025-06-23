#include "HeadwindStormGift.h"
#include <iostream>
#include <cmath>

HeadwindStormGift::HeadwindStormGift(float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    try {
        m_sprite.setTexture(textures.getResource("HeadwindStormGift.png"));
        m_sprite.setPosition(x, y);
        m_sprite.setScale(0.5f, 0.5f);

        // Set origin to center for storm effects
        sf::Vector2u textureSize = m_sprite.getTexture()->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    }
    catch (const std::exception& e) {
        std::cerr << " Error creating HeadwindStormGift: " << e.what() << std::endl;
    }
}

void HeadwindStormGift::render(sf::RenderTarget& target) const {
    if (!m_collected) {
        // Update animation
        updateStormAnimation(0.016f); // Approximate 60 FPS

        // Create storm/wind effect
        sf::Sprite stormSprite = m_sprite;

        // Oscillating movement to simulate wind
        float windOffset = 3.0f * sin(m_stormTimer * 6.0f); // Fast oscillation
        sf::Vector2f originalPos = m_sprite.getPosition();
        stormSprite.setPosition(originalPos.x + windOffset, originalPos.y);

        // Color variation to show storm intensity
        float intensity = 0.7f + 0.3f * sin(m_stormTimer * 4.0f);
        sf::Uint8 grayValue = static_cast<sf::Uint8>(150 + 105 * intensity);

        // Storm colors (gray/blue tones)
        sf::Uint8 red = static_cast<sf::Uint8>(grayValue * 0.8f);
        sf::Uint8 green = static_cast<sf::Uint8>(grayValue * 0.9f);
        sf::Uint8 blue = static_cast<sf::Uint8>(grayValue);

        stormSprite.setColor(sf::Color(red, green, blue, 255));

        // Draw multiple layers for storm effect
        // Background storm layer (larger, more transparent)
        sf::Sprite backgroundStorm = stormSprite;
        backgroundStorm.setScale(0.6f, 0.6f);
        backgroundStorm.setColor(sf::Color(100, 120, 150, 100));
        backgroundStorm.setPosition(originalPos.x - windOffset * 0.5f, originalPos.y);
        target.draw(backgroundStorm);

        // Main storm sprite
        target.draw(stormSprite);

        // Foreground wind lines effect (optional)
        sf::Sprite windLines = stormSprite;
        windLines.setScale(0.3f, 0.3f);
        windLines.setColor(sf::Color(200, 200, 255, 150));
        windLines.setPosition(originalPos.x + windOffset * 1.5f, originalPos.y - 5.0f);
        target.draw(windLines);
    }
}

sf::FloatRect HeadwindStormGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void HeadwindStormGift::onCollect(GameState& gameState) {
    if (!m_collected) {
        m_collected = true;
        gameState.addScore(getValue());
        // يمكن إضافة تأثيرات بيئية:
        // - تغيير سرعة الخلفية
        // - أصوات الرياح
        // - تأثيرات الجسيمات للرياح
    }
}

void HeadwindStormGift::updateStormAnimation(float deltaTime) const {
    // تحديث مؤقت العاصفة
    m_stormTimer += deltaTime;

    // إعادة تعيين المؤقت لتجنب overflow
    if (m_stormTimer > 360.0f) {
        m_stormTimer = 0.0f;
    }
}