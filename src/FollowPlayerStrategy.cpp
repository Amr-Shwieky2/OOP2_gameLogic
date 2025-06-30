// FollowPlayerStrategy.cpp
#include "FollowPlayerStrategy.h"
#include "Entity.h"
#include "PlayerEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

FollowPlayerStrategy::FollowPlayerStrategy(float speed, float detectionRange)
    : m_speed(speed)
    , m_detectionRange(detectionRange) {
    std::cout << "[FOLLOW] Strategy created - Speed: " << speed << " Range: " << detectionRange << std::endl;
}

void FollowPlayerStrategy::update(Entity& entity, float dt, PlayerEntity* player) {
    if (!player) {
        std::cout << "[FOLLOW] No player target!" << std::endl;
        return;
    }

    auto* transform = entity.getComponent<Transform>();
    auto* physics = entity.getComponent<PhysicsComponent>();

    if (!transform || !physics) {
        std::cout << "[FOLLOW] Missing components!" << std::endl;
        return;
    }

    sf::Vector2f entityPos = transform->getPosition();
    sf::Vector2f playerPos = player->getPosition();

    float distance = getDistanceToPlayer(entityPos, playerPos);

    // Debug output every 60 frames
    static int frameCount = 0;
    frameCount++;
    if (frameCount % 60 == 0) {
        std::cout << "[FOLLOW] Enemy " << entity.getId()
            << " Distance to player: " << distance
            << " (Range: " << m_detectionRange << ")" << std::endl;
    }

    // Only follow if player is within detection range
    if (distance <= m_detectionRange) {
        sf::Vector2f direction = getDirectionToPlayer(entityPos, playerPos);

        // Move towards player horizontally
        sf::Vector2f velocity = physics->getVelocity();
        // Convert speed from pixels/sec to Box2D meters/sec
        physics->setVelocity(direction.x * (m_speed / PPM), velocity.y);

        if (frameCount % 60 == 0) {
            std::cout << "[FOLLOW] Chasing player! Direction: " << direction.x
                << " Velocity: " << (direction.x * m_speed) << std::endl;
        }
    }
    else {
        // Stop moving if player is out of range
        sf::Vector2f velocity = physics->getVelocity();
        physics->setVelocity(0, velocity.y);

        if (frameCount % 60 == 0) {
            std::cout << "[FOLLOW] Player out of range, stopping" << std::endl;
        }
    }
}