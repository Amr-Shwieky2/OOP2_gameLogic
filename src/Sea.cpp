#include "Sea.h"
#include <iostream>

Sea::Sea(float x, float y, TextureManager& textures)
    : m_textures(textures) {
    // حفظ المكان الأصلي
    m_originalPosition = sf::Vector2f(x, y);

    try {
        sf::Texture& texture = m_textures.getResource("Sea.png");
        m_sprite.setTexture(texture);
        m_originalColor = sf::Color::White;
    }
    catch (...) {
        std::cout << " Could not load sea.png, using blue rectangle" << std::endl;
    }

    m_sprite.setPosition(x, y);
    m_sprite.setScale(1.0f, 1.0f);
    m_sprite.setColor(m_originalColor);
}

void Sea::render(sf::RenderTarget& target) const {
    // ارسم Sea إذا لم يكن مستهلك أو في عملية الإرجاع
    if (!m_consumed || m_respawning) {
        target.draw(m_sprite);
    }
}

sf::FloatRect Sea::getBounds() const {
    // لا توجد حدود للتصادم إذا كان مستهلك ولا يتم إرجاعه
    if (m_consumed && !m_respawning) {
        return sf::FloatRect(0, 0, 0, 0);
    }
    return m_sprite.getGlobalBounds();
}

void Sea::update(float deltaTime) {
    // ✅ تحديث تأثير الموت
    if (m_deathEffect) {
        m_deathTimer += deltaTime;

        if (m_deathTimer < DEATH_DURATION) {
            // تحول إلى اللون الأحمر وتلاشي
            float progress = m_deathTimer / DEATH_DURATION;
            sf::Uint8 red = static_cast<sf::Uint8>(255 * progress);
            sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (1.0f - progress));

            m_sprite.setColor(sf::Color(red, 0, 0, alpha));
        }
        else {
            // انتهاء تأثير الموت - ابدأ الإرجاع
            m_deathEffect = false;
            startRespawn();
        }
    }

    // ✅ تحديث عملية الإرجاع
    if (m_respawning) {
        m_respawnTimer += deltaTime;

        if (m_respawnTimer < RESPAWN_TIME) {
            // تأثير الإرجاع التدريجي
            float progress = m_respawnTimer / RESPAWN_TIME;

            // يظهر تدريجياً باللون الأزرق الفاتح
            sf::Uint8 alpha = static_cast<sf::Uint8>(255 * progress);
            sf::Color respawnColor = sf::Color(100, 150, 255, alpha); // أزرق فاتح
            m_sprite.setColor(respawnColor);

            // تأثير نبضات (اختياري)
            float pulseScale = 1.0f + 0.1f * std::sin(m_respawnTimer * 10.0f);
            m_sprite.setScale(pulseScale, pulseScale);
        }
        else {
            // انتهاء الإرجاع - Sea يعود كما كان
            m_consumed = false;
            m_respawning = false;
            m_respawnTimer = 0.0f;

            // إعادة الألوان والحجم الأصلي
            m_sprite.setColor(m_originalColor);
            m_sprite.setScale(1.0f, 1.0f);
        }
    }
}

void Sea::consume() {
    m_consumed = true;
}

void Sea::setDeathEffect() {
    m_deathEffect = true;
    m_deathTimer = 0.0f;
}

void Sea::startRespawn() {
    m_respawning = true;
    m_respawnTimer = 0.0f;
    m_sprite.setPosition(m_originalPosition);
}