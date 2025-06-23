#include "ReverseMovementGift.h"
#include <iostream>
#include <cmath>

ReverseMovementGift::ReverseMovementGift(float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    try {
        m_sprite.setTexture(textures.getResource("ReverseMovementGift.png"));
        m_sprite.setPosition(x, y);
        m_sprite.setScale(0.5f, 0.5f);

        // Set origin to center for rotation effects
        sf::Vector2u textureSize = m_sprite.getTexture()->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    }
    catch (const std::exception& e) {
        std::cerr << " Error creating ReverseMovementGift: " << e.what() << std::endl;
    }
}

void ReverseMovementGift::render(sf::RenderTarget& target) const {
    if (!m_collected) {
        // Update animation
        updateAnimation(0.016f); // Approximate 60 FPS

        // Create spinning effect to indicate confusion/reverse
        sf::Sprite animatedSprite = m_sprite;

        // Continuous rotation to show "reverse" nature
        float rotationSpeed = 90.0f; // degrees per second
        animatedSprite.setRotation(m_rotationTimer * rotationSpeed);

        // Add color shift effect (red tint for danger/confusion)
        float colorShift = 0.8f + 0.2f * sin(m_rotationTimer * 4.0f);
        sf::Uint8 red = static_cast<sf::Uint8>(255 * colorShift);
        sf::Uint8 green = static_cast<sf::Uint8>(200 * (1.0f - colorShift * 0.3f));
        sf::Uint8 blue = static_cast<sf::Uint8>(200 * (1.0f - colorShift * 0.3f));

        animatedSprite.setColor(sf::Color(red, green, blue, 255));

        target.draw(animatedSprite);
    }
}

sf::FloatRect ReverseMovementGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void ReverseMovementGift::onCollect(GameState& gameState) {
    if (!m_collected) {
        m_collected = true;
        gameState.addScore(getValue());
        // يمكن إضافة تحذيرات بصرية:
        // - وميض الشاشة
        // - تغيير لون UI مؤقتاً
        // - رسالة تحذير
    }
}

void ReverseMovementGift::updateAnimation(float deltaTime) const {
    // تحديث مؤقت الدوران
    m_rotationTimer += deltaTime;

    // إعادة تعيين المؤقت لتجنب overflow
    if (m_rotationTimer > 360.0f) {
        m_rotationTimer = 0.0f;
    }
}