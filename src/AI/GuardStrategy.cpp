#include "GuardStrategy.h"
#include "Entity.h"
#include "PlayerEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include "Constants.h"
#include <iostream>

//-------------------------------------------------------------------------------------
GuardStrategy::GuardStrategy(float guardRadius, float attackRange)
    : m_guardRadius(guardRadius)
    , m_attackRange(attackRange) {
}
//-------------------------------------------------------------------------------------
void GuardStrategy::update(Entity& entity, float dt, PlayerEntity* player) {
    if (!player) return;

    auto* transform = entity.getComponent<Transform>();
    auto* physics = entity.getComponent<PhysicsComponent>();

    if (!transform || !physics) return;

    // Initialize guard position
    if (!m_initialized) {
        m_guardPosition = transform->getPosition();
        m_initialized = true;
    }

    sf::Vector2f entityPos = transform->getPosition();
    sf::Vector2f playerPos = player->getPosition();

    // Update attack cooldown
    if (m_attackCooldown > 0) {
        m_attackCooldown -= dt;
    }

    float distanceToPlayer = getDistanceToPlayer(entityPos, playerPos);
    float distanceToGuardPos = getDistanceToPlayer(entityPos, m_guardPosition);

    // If player is within attack range, attack (stay in place)
    if (distanceToPlayer <= m_attackRange) {
        // Stop moving to attack
        physics->setVelocity(0, physics->getVelocity().y);

        // Attack logic (could shoot projectiles here)
        if (m_attackCooldown <= 0) {
            std::cout << "[Guard] Attacking player!" << std::endl;
            m_attackCooldown = 1.0f; // 1 second between attacks
        }
    }
    // If player is within guard radius, move towards them
    else if (distanceToPlayer <= m_guardRadius) {
        sf::Vector2f direction = getDirectionToPlayer(entityPos, playerPos);
        // Convert speeds to Box2D meters/sec
        physics->setVelocity(direction.x * (80.0f / PPM), physics->getVelocity().y);
    }
    // Otherwise, return to guard position
    else if (distanceToGuardPos > 10.0f) {
        sf::Vector2f direction = getDirectionToPlayer(entityPos, m_guardPosition);
        physics->setVelocity(direction.x * (50.0f / PPM), physics->getVelocity().y);
    }
    else {
        // At guard position, stand still
        physics->setVelocity(0, physics->getVelocity().y);
    }
}
//-------------------------------------------------------------------------------------