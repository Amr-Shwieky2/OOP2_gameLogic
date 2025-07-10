#pragma once
#include <memory>
#include "EntityManager.h"
#include "PhysicsManager.h"
#include "CollisionManager.h"
#include "GameLevelManager.h"
#include "GameEventCoordinator.h"
#include "EntityCleanupManager.h"
#include "RenderSystem.h"
#include "SurpriseBoxManager.h"
#include "PlayerEntity.h"
#include "ResourceManager.h"


// Global pointer for backwards compatibility
extern class GameSession* g_currentSession;

/**
 * GameSession - Single Responsibility: Coordinate all game subsystems
 *
 * This class now ONLY coordinates other managers - it doesn't do the work itself!
 * Perfect SRP compliance.
 */
class GameSession {
public:
    GameSession();
    ~GameSession();

    void initialize(TextureManager& textures, sf::RenderWindow& window);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    // Simple delegation to managers - no business logic here!
    PlayerEntity* getPlayer();
    EntityManager& getEntityManager() { return m_entityManager; }
    void spawnEntity(std::unique_ptr<Entity> entity);

    // Level operations (delegated to GameLevelManager)
    bool loadLevel(const std::string& levelPath);
    bool loadNextLevel();
    void reloadCurrentLevel();
    const std::string& getCurrentLevelName() const;

    // Invalidate cached player pointer (e.g., when levels reload internally)
    void invalidateCachedPlayer();

    // Other accessors
    SurpriseBoxManager* getSurpriseBoxManager() { return m_surpriseBoxManager.get(); }
    GameLevelManager& getLevelManager() { return m_levelManager; }

private:
    // All the single-responsibility managers
    EntityManager m_entityManager;
    PhysicsManager m_physicsManager;
    CollisionManager m_collisionManager;
    GameLevelManager m_levelManager;
    GameEventCoordinator m_eventCoordinator;
    EntityCleanupManager m_cleanupManager;
    RenderSystem m_renderSystem;

    std::unique_ptr<SurpriseBoxManager> m_surpriseBoxManager;

    // Simple cache for quick access
    PlayerEntity* m_player = nullptr;
    TextureManager* m_textures = nullptr;

    // Helper methods - these just coordinate, don't do work
    void findAndCachePlayer();
    void updateAllSubsystems(float deltaTime);
};
