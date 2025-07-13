#pragma once
#include "Component.h"
#include <SFML/System/Vector2.hpp>

class Transform : public Component {
public:
    Transform();
    Transform(const sf::Vector2f& position);

    // Position
    void setPosition(const sf::Vector2f& pos);
    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;

    void move(const sf::Vector2f& delta);
    void move(float dx, float dy);

    // Rotation (in degrees)
    void setRotation(float angle);
    float getRotation() const;
    void rotate(float delta);

    // Scale
    void setScale(const sf::Vector2f& scale);
    void setScale(float x, float y);
    sf::Vector2f getScale() const;
    void scale(float factorX, float factorY);

private:
    sf::Vector2f m_position{ 0.f, 0.f };
    float m_rotation{ 0.f }; // Degrees
    sf::Vector2f m_scale{ 1.f, 1.f };
};