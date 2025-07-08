#include "EntityCleanupManager.h"
#include "EntityManager.h"
#include "Entity.h"
#include <iostream>

EntityCleanupManager::EntityCleanupManager() {
    std::cout << "[EntityCleanupManager] Created" << std::endl;
}

void EntityCleanupManager::update(float deltaTime) {
    m_cleanupTimer += deltaTime;

    // Automatic cleanup at intervals
    if (m_cleanupTimer >= m_cleanupInterval || m_immediateCleanup) {
        m_cleanupTimer = 0.0f;

        // Clear scheduled entities (they'll be cleaned up by EntityManager)
        if (!m_scheduledForCleanup.empty()) {
            std::cout << "[EntityCleanupManager] Clearing " << m_scheduledForCleanup.size()
                << " scheduled entities" << std::endl;
            m_scheduledForCleanup.clear();
        }
    }
}

void EntityCleanupManager::cleanupInactiveEntities(EntityManager& entityManager) {
    m_lastCleanupCount = 0;

    // Get all entities and check which are inactive
    auto entities = entityManager.getAllEntities();
    std::vector<Entity::IdType> toRemove;

    for (auto* entity : entities) {
        if (!entity->isActive()) {
            toRemove.push_back(entity->getId());
            m_lastCleanupCount++;
        }
    }

    // Remove inactive entities
    for (auto id : toRemove) {
        entityManager.destroyEntity(id);
    }

    if (m_lastCleanupCount > 0) {
        std::cout << "[EntityCleanupManager] Cleaned up " << m_lastCleanupCount
            << " inactive entities" << std::endl;
    }

    // Use EntityManager's built-in cleanup
    entityManager.removeInactiveEntities();
}

void EntityCleanupManager::scheduleForCleanup(Entity* entity) {
    if (entity) {
        m_scheduledForCleanup.push_back(entity);
        std::cout << "[EntityCleanupManager] Scheduled entity " << entity->getId()
            << " for cleanup" << std::endl;
    }
}

void EntityCleanupManager::forceCleanup(EntityManager& entityManager) {
    std::cout << "[EntityCleanupManager] Force cleanup requested" << std::endl;
    cleanupInactiveEntities(entityManager);
    m_cleanupTimer = 0.0f; // Reset timer
}
