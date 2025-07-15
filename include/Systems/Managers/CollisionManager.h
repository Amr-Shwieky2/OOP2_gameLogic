#pragma once
#include "MultiMethodCollisionSystem.h"
#include <vector>

class Entity;
class EntityManager;

/**
 * CollisionManager - Single Responsibility: Detect and resolve collisions
 */
class CollisionManager {
public:
    CollisionManager();

    void setupGameCollisionHandlers();
    void checkCollisions(EntityManager& entityManager);
    void clearHandlers();

    // Debug/Statistics
    int getCollisionCheckCount() const { return m_collisionChecks; }
    int getCollisionCount() const { return m_collisionsProcessed; }
    void resetStats();

private:
    MultiMethodCollisionSystem m_collisionSystem;
    int m_collisionChecks = 0;
    int m_collisionsProcessed = 0;

    bool areColliding(Entity& a, Entity& b) const;
};