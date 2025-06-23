#include "ProtectiveShieldGift.h"
#include <iostream>
#include <cmath>

ProtectiveShieldGift::ProtectiveShieldGift(float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    try {
        m_sprite.setTexture(textures.getResource("ProtectiveShieldGift.png"));
        m_sprite.setPosition(x, y);
        m_sprite.setScale(0.5f, 0.5f);

        sf::Vector2u textureSize = m_sprite.getTexture()->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);

        setupShieldEffects();
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating ProtectiveShieldGift: " << e.what() << std::endl;
    }
}

void ProtectiveShieldGift::render(sf::RenderTarget& target) const {
    if (!m_collected) {
        // Update animation
        updateAnimation(0.016f); // Approximate 60 FPS

        // Draw shield with protective aura effect
        sf::Sprite auraSprite = m_sprite;

        // Create pulsing aura effect
        float pulseIntensity = 0.8f + 0.2f * sin(m_animationTimer * 3.0f);
        sf::Uint8 auraAlpha = static_cast<sf::Uint8>(100 * pulseIntensity);

        // Draw larger, semi-transparent aura
        auraSprite.setScale(0.6f, 0.6f); // Slightly larger than main sprite
        auraSprite.setColor(sf::Color(0, 150, 255, auraAlpha)); // Blue protective aura
        target.draw(auraSprite);

        // Draw smaller inner aura
        auraSprite.setScale(0.55f, 0.55f);
        auraSprite.setColor(sf::Color(100, 200, 255, auraAlpha + 50));
        target.draw(auraSprite);

        // Draw main sprite with slight brightness variation
        sf::Sprite mainSprite = m_sprite;
        sf::Uint8 brightness = static_cast<sf::Uint8>(200 + 55 * pulseIntensity);
        mainSprite.setColor(sf::Color(brightness, brightness, 255, 255));
        target.draw(mainSprite);
    }
}

sf::FloatRect ProtectiveShieldGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void ProtectiveShieldGift::onCollect(GameState& gameState) {
    if (!m_collected) {
        m_collected = true;
        gameState.addScore(getValue());

        // يمكن إضافة تأثيرات إضافية:
        // - صوت جمع الدرع
        // - تأثيرات بصرية للحماية
        // - إشعار UI للحماية النشطة
    }
}

void ProtectiveShieldGift::setupShieldEffects() {
    // تهيئة التأثيرات البصرية
    m_animationTimer = 0.0f;

    // يمكن إضافة المزيد من الإعدادات هنا:
    // - ألوان التأثيرات
    // - سرعة الأنيميشن
    // - قوة التوهج
}

void ProtectiveShieldGift::updateAnimation(float deltaTime) const {
    // تحديث مؤقت الأنيميشن
    m_animationTimer += deltaTime;

    // إعادة تعيين المؤقت لتجنب overflow
    if (m_animationTimer > 360.0f) {
        m_animationTimer = 0.0f;
    }
}
