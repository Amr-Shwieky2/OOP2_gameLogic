#include "Sea.h"
#include "Player.h"
#include <iostream>

Sea::Sea(float x, float y, TextureManager& textures)
    : m_textures(textures) {

    m_originalPosition = sf::Vector2f(x, y);

    // تحميل صورة البحر
    try {
        sf::Texture& texture = m_textures.getResource("Sea.png");
        m_sprite.setTexture(texture);
        m_originalColor = sf::Color::White;
    }
    catch (...) {
        std::cout << "Could not load Sea.png" << std::endl;
        m_originalColor = sf::Color(0, 100, 200); // أزرق افتراضي
    }

    m_sprite.setPosition(m_originalPosition);
    m_sprite.setColor(m_originalColor);
}

void Sea::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect Sea::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void Sea::update(float deltaTime) {
    // تحديث cooldown timer
    if (m_cooldownTimer > 0.0f) {
        m_cooldownTimer -= deltaTime;
        if (m_cooldownTimer <= 0.0f) {
            m_canHarm = true;
        }
    }
}

void Sea::onPlayerContact(Player& player) {
    // تحقق من إمكانية الإيذاء
    if (!m_canHarm) {
        return;
    }
    // قتل اللاعب فوراً (استخدام الدالة الجديدة)
    player.kill();

    // تعطيل الإيذاء لفترة قصيرة لمنع التكرار
    m_canHarm = false;
    m_cooldownTimer = HARM_COOLDOWN;
}