#include "LifeHeartGift.h"
#include <iostream>
#include <cmath>

LifeHeartGift::LifeHeartGift(float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    try {
        m_sprite.setTexture(textures.getResource("LifeHeartGift.png"));
        m_sprite.setPosition(x, y);
        m_sprite.setScale(0.5f, 0.5f);

        // Set origin to center for better animation
        sf::Vector2u textureSize = m_sprite.getTexture()->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);

        setupHeartAnimation();
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating LifeHeartGift: " << e.what() << std::endl;
    }
}

void LifeHeartGift::render(sf::RenderTarget& target) const {
    if (!m_collected) {
        target.draw(m_sprite);
    }
}

sf::FloatRect LifeHeartGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void LifeHeartGift::onCollect(GameState& gameState) {
    if (!m_collected) {
        m_collected = true;
        gameState.addLife();

        // يمكن إضافة تأثيرات بصرية/صوتية هنا
        // - تشغيل صوت
        // - إظهار تأثير جمع
        // - إضافة نقاط إضافية

        // إضافة نقاط إضافية لجمع القلب
        gameState.addScore(25); // مكافأة إضافية

    }
}

void LifeHeartGift::setupHeartAnimation() {
    // يمكن إضافة تأثيرات بصرية هنا لاحقاً
    // مثل النبض أو التوهج
}
