#include "MovementComponent.h"
#include "Entity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include <cmath>

MovementComponent::MovementComponent(MovementType type)
    : m_type(type) {
}

void MovementComponent::update(float dt) {
    if (!m_owner) return;

    auto* transform = m_owner->getComponent<Transform>();
    auto* physics = m_owner->getComponent<PhysicsComponent>();

    if (!transform) return;

    switch (m_type) {
    case MovementType::Linear: {
        if (physics) {
            // Use physics for movement
            physics->setVelocity(m_direction.x * m_speed, m_direction.y * m_speed);
        }
        else {
            // Direct transform movement
            sf::Vector2f movement = m_direction * m_speed * dt;
            transform->move(movement);
        }
        break;
    }

    case MovementType::Following: {
        sf::Vector2f currentPos = transform->getPosition();
        sf::Vector2f toTarget = m_target - currentPos;
        float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

        if (distance > 1.0f) {
            toTarget /= distance; // Normalize

            if (physics) {
                physics->setVelocity(toTarget.x * m_speed, toTarget.y * m_speed);
            }
            else {
                sf::Vector2f movement = toTarget * m_speed * dt;
                transform->move(movement);
            }
        }
        break;
    }

    case MovementType::Circular: {
        m_angle += m_speed * dt;

        float x = m_circleCenter.x + m_circleRadius * std::cos(m_angle);
        float y = m_circleCenter.y + m_circleRadius * std::sin(m_angle);

        if (physics) {
            physics->setPosition(x, y);
        }
        else {
            transform->setPosition(x, y);
        }
        break;
    }

    case MovementType::Sine: {
        // Sine wave movement along x-axis
        sf::Vector2f pos = transform->getPosition();
        pos.x += m_speed * dt;
        pos.y = m_circleCenter.y + m_circleRadius * std::sin(pos.x * 0.01f);

        if (physics) {
            physics->setPosition(pos.x, pos.y);
        }
        else {
            transform->setPosition(pos);
        }
        break;
    }

    case MovementType::Static:
    case MovementType::Custom:
    default:
        // No automatic movement
        break;
    }
}

void MovementComponent::setCircularMotion(const sf::Vector2f& center, float radius, float speed) {
    m_type = MovementType::Circular;
    m_circleCenter = center;
    m_circleRadius = radius;
    m_speed = speed;
    m_angle = 0.0f;
}