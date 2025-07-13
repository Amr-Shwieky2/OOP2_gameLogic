#pragma once
#include "Component.h"
#include <SFML/System/Vector2.hpp>

/**
 * MovementComponent - Handles movement patterns
 * Can be used for enemies, moving platforms, etc.
 */
class MovementComponent : public Component {
public:
    enum class MovementType {
        Static,
        Linear,
        Following,
        Circular,
        Sine,
        Custom
    };

    MovementComponent(MovementType type = MovementType::Static);

    void update(float dt) override;

    // Movement configuration
    void setSpeed(float speed) { m_speed = speed; }
    void setDirection(const sf::Vector2f& dir) { m_direction = dir; }
    void setTarget(const sf::Vector2f& target) { m_target = target; }

    // For circular movement
    void setCircularMotion(const sf::Vector2f& center, float radius, float speed);

private:
    MovementType m_type;
    float m_speed = 100.0f;
    sf::Vector2f m_direction{ 1.0f, 0.0f };
    sf::Vector2f m_target;

    // Circular motion parameters
    sf::Vector2f m_circleCenter;
    float m_circleRadius = 0.0f;
    float m_angle = 0.0f;
};