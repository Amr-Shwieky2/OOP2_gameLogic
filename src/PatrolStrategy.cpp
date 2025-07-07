// PatrolStrategy.cpp
#include "PatrolStrategy.h"
#include "Entity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

PatrolStrategy::PatrolStrategy(float patrolDistance, float speed)
    : m_patrolDistance(patrolDistance)
    , m_speed(speed) {
}

void PatrolStrategy::update(Entity& entity, float dt, PlayerEntity* player) {
    auto* transform = entity.getComponent<Transform>();
    auto* physics = entity.getComponent<PhysicsComponent>();

    if (!transform || !physics) {
        return;
    }

    // Initialize starting position
    if (!m_initialized) {
        m_startX = transform->getPosition().x;
        m_initialized = true;
    }

    sf::Vector2f currentPos = transform->getPosition();

    // Check if we need to turn around
    float distanceFromStart = currentPos.x - m_startX;

    // Change direction at patrol boundaries
    if (std::abs(distanceFromStart) >= m_patrolDistance) {
        m_direction *= -1;
    }

    // Move in current direction
    sf::Vector2f velocity = physics->getVelocity();
    // Convert speed from pixels/sec to Box2D meters/sec
    physics->setVelocity(m_direction * (m_speed / PPM), velocity.y);
}