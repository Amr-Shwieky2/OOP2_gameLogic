#include "Coin.h"
#include <iostream>

Coin::Coin(float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    try {
        sf::Texture& tex = m_textures.getResource("Coin.png");
        m_sprite.setTexture(tex);
    }
    catch (...) {
        std::cout << " Could not load coin.png" << std::endl;
    }
    m_sprite.setScale(0.08f, 0.08f);
    //  حفظ المركز الأصلي للدوران
    m_centerPosition = sf::Vector2f(x, y);
    //  بدء الموقع في الدائرة
    float startX = m_centerPosition.x + m_circularRadius * std::cos(m_angle);
    float startY = m_centerPosition.y + m_circularRadius * std::sin(m_angle);
    m_sprite.setPosition(startX, startY);

    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
}

void Coin::render(sf::RenderTarget& target) const {
    if (!m_collected) {
        target.draw(m_sprite);
    }
}

sf::FloatRect Coin::getBounds() const {
    if (m_collected) {
        return sf::FloatRect(0, 0, 0, 0);
    }
    return m_sprite.getGlobalBounds();
}

void Coin::update(float deltaTime) {
    if (m_collected) return;
    //  تحديث الحركة الدائرية
    if (m_enableCircular) {
        // تحديث الزاوية
        m_angle += m_circularSpeed * deltaTime;

        // إبقاء الزاوية بين 0 و 2π
        if (m_angle > 2.0f * M_PI) {
            m_angle -= 2.0f * M_PI;
        }
        // حساب الموقع الجديد
        float newX = m_centerPosition.x + m_circularRadius * std::cos(m_angle);
        float newY = m_centerPosition.y + m_circularRadius * std::sin(m_angle);
        //  إضافة حركة صعود ونزول للجمالية
        m_bobOffset += 3.0f * deltaTime;
        float bobY = 5.0f * std::sin(m_bobOffset);

        m_sprite.setPosition(newX, newY + bobY);
    }
    //  دوران الـ sprite حول نفسه
    m_rotationAngle += 90.0f * deltaTime; // 90 درجة في الثانية
    if (m_rotationAngle > 360.0f) {
        m_rotationAngle -= 360.0f;
    }
    m_sprite.setRotation(m_rotationAngle);
    //  تأثير نبضات في الحجم
    float scaleVariation = 0.08f + 0.01f * std::sin(m_bobOffset * 2.0f);
    m_sprite.setScale(scaleVariation, scaleVariation);
}

void Coin::collect() {
    m_collected = true;
}

bool Coin::isCollected() const {
    return m_collected;
}

void Coin::setCircularMovement(float radius, float speed) {
    m_circularRadius = radius;
    m_circularSpeed = speed;
    m_enableCircular = true;
}
void Coin::disableCircularMovement() {
    m_enableCircular = false;
    m_sprite.setPosition(m_centerPosition);
}

void Coin::moveTowards(sf::Vector2f target, float speed) {
    m_enableCircular = false;
    sf::Vector2f pos = m_sprite.getPosition();
    sf::Vector2f dir = target - pos;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 20.f) {
        collect(); 
        return;
    }
    if (len > 0.f) {
        dir /= len;
        m_sprite.move(dir * speed);
    }
}

sf::Vector2f Coin::getPosition() const {
    return m_sprite.getPosition();
}