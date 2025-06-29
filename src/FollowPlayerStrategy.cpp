// FollowPlayerStrategy.cpp
#include "FollowPlayerStrategy.h"
#include "Entity.h"
#include "PlayerEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include <iostream>

FollowPlayerStrategy::FollowPlayerStrategy(float speed, float detectionRange)
    : m_speed(speed)
    , m_detectionRange(detectionRange) {
}

void FollowPlayerStrategy::update(Entity& entity, float dt, PlayerEntity* player) {
    if (!player) return;

    auto* transform = entity.getComponent<Transform>();
    auto* physics = entity.getComponent<PhysicsComponent>();

    if (!transform || !physics) return;

    sf::Vector2f entityPos = transform->getPosition();
    sf::Vector2f playerPos = player->getPosition();

    float distance = getDistanceToPlayer(entityPos, playerPos);

    // Only follow if player is within detection range
    if (distance <= m_detectionRange) {
        sf::Vector2f direction = getDirectionToPlayer(entityPos, playerPos);

        // Move towards player
        sf::Vector2f velocity = physics->getVelocity();
        physics->setVelocity(direction.x * m_speed, velocity.y);
    }
    else {
        // Stop moving if player is out of range
        sf::Vector2f velocity = physics->getVelocity();
        physics->setVelocity(0, velocity.y);
    }
}