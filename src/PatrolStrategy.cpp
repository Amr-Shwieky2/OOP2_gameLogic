// PatrolStrategy.cpp
#include "PatrolStrategy.h"
#include "Entity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include <iostream>
#include <cmath>

PatrolStrategy::PatrolStrategy(float patrolDistance, float speed)
    : m_patrolDistance(patrolDistance)
    , m_speed(speed) {
    std::cout << "[PATROL] Strategy created - Distance: " << patrolDistance << " Speed: " << speed << std::endl;
}

void PatrolStrategy::update(Entity& entity, float dt, PlayerEntity* player) {
    auto* transform = entity.getComponent<Transform>();
    auto* physics = entity.getComponent<PhysicsComponent>();

    if (!transform || !physics) {
        std::cout << "[PATROL] Missing transform or physics component!" << std::endl;
        return;
    }

    // Initialize starting position
    if (!m_initialized) {
        m_startX = transform->getPosition().x;
        m_initialized = true;
        std::cout << "[PATROL] Initialized at X position: " << m_startX << std::endl;
    }

    sf::Vector2f currentPos = transform->getPosition();

    // Check if we need to turn around
    float distanceFromStart = currentPos.x - m_startX;

    // Change direction at patrol boundaries
    if (std::abs(distanceFromStart) >= m_patrolDistance) {
        m_direction *= -1;
        std::cout << "[PATROL] Changing direction at distance: " << distanceFromStart << std::endl;
    }

    // Move in current direction
    sf::Vector2f velocity = physics->getVelocity();
    physics->setVelocity(m_direction * m_speed, velocity.y);

    // Debug output every 60 frames (about once per second)
    static int frameCount = 0;
    frameCount++;
    if (frameCount % 60 == 0) {
        std::cout << "[PATROL] Enemy " << entity.getId()
            << " Pos: " << currentPos.x
            << " Dir: " << m_direction
            << " Vel: " << (m_direction * m_speed) << std::endl;
    }
}