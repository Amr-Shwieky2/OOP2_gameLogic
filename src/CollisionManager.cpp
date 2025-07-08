#include "CollisionManager.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Transform.h"
#include "GameCollisionSetup.h"
#include <cmath>
#include <iostream>

CollisionManager::CollisionManager() {
    std::cout << "[CollisionManager] Created" << std::endl;
}

void CollisionManager::setupGameCollisionHandlers() {
    ::setupGameCollisionHandlers(m_collisionSystem); 
    std::cout << "[CollisionManager] Game collision handlers setup complete" << std::endl;
}

void CollisionManager::checkCollisions(EntityManager& entityManager) {
    m_collisionChecks = 0;
    m_collisionsProcessed = 0;

    auto entities = entityManager.getAllEntities();

    for (size_t i = 0; i < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
            if (entities[i]->isActive() && entities[j]->isActive()) {
                m_collisionChecks++;

                if (areColliding(*entities[i], *entities[j])) {
                    if (m_collisionSystem.processCollision(*entities[i], *entities[j])) {
                        m_collisionsProcessed++;
                    }
                }
            }
        }
    }
}

bool CollisionManager::areColliding(Entity& a, Entity& b) const {
    auto* transA = a.getComponent<Transform>();
    auto* transB = b.getComponent<Transform>();

    if (!transA || !transB) return false;

    sf::Vector2f posA = transA->getPosition();
    sf::Vector2f posB = transB->getPosition();

    float dx = posA.x - posB.x;
    float dy = posA.y - posB.y;
    float distSq = dx * dx + dy * dy;

    float collisionDistance = 100.0f; // Default collision distance
    return distSq < (collisionDistance * collisionDistance);
}

void CollisionManager::clearHandlers() {
    m_collisionSystem.clear();
}

void CollisionManager::resetStats() {
    m_collisionChecks = 0;
    m_collisionsProcessed = 0;
}