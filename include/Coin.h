#pragma once
#include "DynamicGameObject.h"
#include "ResourceManager.h"
#include "Player.h"
#include <cmath>

class Coin : public DynamicGameObject {
public:
    Coin(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void update(float deltaTime) override;

    void collect();
    bool isCollected() const;
    void setCircularMovement(float radius = 30.0f, float speed = 2.0f);
    void disableCircularMovement();

    void moveTowards(sf::Vector2f target, float speed);

    sf::Vector2f getPosition() const;

private:
    sf::Sprite m_sprite;
    bool m_collected = false;
    TextureManager& m_textures;

    //  متغيرات الحركة الدائرية
    sf::Vector2f m_centerPosition;    // المركز الأصلي للدوران
    float m_circularRadius = 30.0f;   // نصف قطر الدوران
    float m_circularSpeed = 2.0f;     // سرعة الدوران
    float m_angle = 0.0f;             // الزاوية الحالية
    bool m_enableCircular = true;     // تفعيل الحركة الدائرية

    // تأثيرات بصرية إضافية
    float m_bobOffset = 0.0f;         // حركة صعود ونزول
    float m_rotationAngle = 0.0f;     // دوران الـ sprite نفسه

    static constexpr float M_PI = 3.14159265358979323846f;

};