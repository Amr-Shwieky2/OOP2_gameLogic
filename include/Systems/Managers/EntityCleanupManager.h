#pragma once
#include <vector>

class Entity;
class EntityManager;

/**
 * EntityCleanupManager - Single Responsibility: Manage entity lifecycle cleanup
 */
class EntityCleanupManager {
public:
    EntityCleanupManager();

    void update(float deltaTime);
    void cleanupInactiveEntities(EntityManager& entityManager);
    void scheduleForCleanup(Entity* entity);
    void forceCleanup(EntityManager& entityManager);

    // Configuration
    void setCleanupInterval(float interval) { m_cleanupInterval = interval; }
    void setImmediateCleanup(bool immediate) { m_immediateCleanup = immediate; }

    // Statistics
    int getLastCleanupCount() const { return m_lastCleanupCount; }

private:
    float m_cleanupTimer = 0.0f;
    float m_cleanupInterval = 1.0f; // Clean every second
    bool m_immediateCleanup = false;
    int m_lastCleanupCount = 0;

    std::vector<Entity*> m_scheduledForCleanup;
};