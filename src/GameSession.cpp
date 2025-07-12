#include "GameSession.h"
#include "PlayerEntity.h"
#include "GameCollisionSetup.h"
#include "ResourceManager.h"
#include "FalconEnemyEntity.h"
#include "Transform.h"
#include "Constants.h"
#include <memory>
#include <iostream>

// Global pointer for backwards compatibility
GameSession* g_currentSession = nullptr;
extern int g_nextEntityId;

GameSession::GameSession() {
    g_currentSession = this;
}

GameSession::~GameSession() {
    // FIRST: Clear global pointer to prevent any access
    if (g_currentSession == this) {
        g_currentSession = nullptr;
    }

    try {
        // Clear player cache immediately
        m_player = nullptr;

        // Shutdown surprise box manager FIRST (it has event subscriptions)
        if (m_surpriseBoxManager) {
            m_surpriseBoxManager->reset();
            m_surpriseBoxManager.reset();
        }

        // Clear all event handlers
        m_eventCoordinator.shutdown();

        // Clear collision system
        m_collisionManager.clearHandlers();

        // Force cleanup of entities
        m_cleanupManager.forceCleanup(m_entityManager);

        // Clear all entities
        m_entityManager.clear();

        // Clear any remaining event system listeners
        EventSystem::getInstance().clear();
    }
    catch (const std::exception& e) {
        // Silently handle the exception - removed std::cout
    }
}

void GameSession::initialize(TextureManager& textures, sf::RenderWindow& window) {
    m_textures = &textures;

    // Initialize all managers in proper order
    // 1. Physics first (others depend on it)
    // Physics manager initializes itself in constructor

    // 2. Level manager (needs physics and entity manager)
    m_levelManager.initialize(m_entityManager, m_physicsManager, textures);

    // 3. Collision system
    m_collisionManager.setupGameCollisionHandlers();

    // 4. Event system
    m_eventCoordinator.initialize();

    // 5. Register entities for factory (needed for level loading)
    registerGameEntities(m_physicsManager.getWorld(), textures);

    // 6. Setup surprise box manager
    m_surpriseBoxManager = std::make_unique<SurpriseBoxManager>(textures, window);
    m_surpriseBoxManager->setEntityManager(&m_entityManager);
    m_surpriseBoxManager->setPhysicsWorld(&m_physicsManager.getWorld());
}

void GameSession::update(float deltaTime) {
    // Simply coordinate all subsystems - no business logic here!
    updateAllSubsystems(deltaTime);
}

void GameSession::render(sf::RenderWindow& window) {
    // Delegate to render system
    m_renderSystem.render(m_entityManager, window);
}

PlayerEntity* GameSession::getPlayer() {
    if (!m_player) {
        findAndCachePlayer();
    }
    return m_player;
}

void GameSession::invalidateCachedPlayer() {
    m_player = nullptr;
}

void GameSession::spawnEntity(std::unique_ptr<Entity> entity) {
    if (entity) {
        // Check if it's a player for caching
        if (auto* player = dynamic_cast<PlayerEntity*>(entity.get())) {
            m_player = player;

            // Setup surprise box manager with player
            if (m_surpriseBoxManager) {
                m_surpriseBoxManager->setPlayer(m_player);
            }
        }

        // Delegate to entity manager
        m_entityManager.addEntity(std::move(entity));
    }
}

bool GameSession::loadLevel(const std::string& levelPath) {
    m_player = nullptr; // Reset player cache
    m_falconSpawnTimer = 0.f;
    m_falconSpawned = false;
    // Delegate to level manager
    return m_levelManager.loadLevel(levelPath);
}

bool GameSession::loadNextLevel() {
    m_player = nullptr; // Reset player cache
    m_falconSpawnTimer = 0.f;
    m_falconSpawned = false;
    // Delegate to level manager
    return m_levelManager.loadNextLevel();
}

void GameSession::reloadCurrentLevel() {
    m_player = nullptr; // Reset player cache
    m_falconSpawnTimer = 0.f;
    m_falconSpawned = false;
    // Delegate to level manager
    m_levelManager.reloadCurrentLevel();
}

const std::string& GameSession::getCurrentLevelName() const {
    // Delegate to level manager
    return m_levelManager.getCurrentLevelPath();
}

void GameSession::findAndCachePlayer() {
    // Simple player finding - no complex logic
    for (auto* entity : m_entityManager.getAllEntities()) {
        if (auto* player = dynamic_cast<PlayerEntity*>(entity)) {
            m_player = player;

            // Setup surprise box manager with player
            if (m_surpriseBoxManager) {
                m_surpriseBoxManager->setPlayer(m_player);
            }
            break;
        }
    }
}

void GameSession::updateAllSubsystems(float deltaTime) {
    // Coordinate all managers in proper order - no business logic!

    // 1. Update physics world
    m_physicsManager.update(deltaTime);

    // 2. Update level manager (handles transitions)
    m_levelManager.update(deltaTime);

    // Check timed spawns
    updateFalconSpawner(deltaTime);

    // 3. Update all entities
    m_entityManager.updateAll(deltaTime);

    // 4. Check collisions
    m_collisionManager.checkCollisions(m_entityManager);

    // 5. Cleanup inactive entities
    m_cleanupManager.update(deltaTime);
    m_cleanupManager.cleanupInactiveEntities(m_entityManager);
}

void GameSession::updateFalconSpawner(float deltaTime) {
    if (m_falconSpawned)
        return;

    m_falconSpawnTimer += deltaTime;
    if (m_falconSpawnTimer >= 30.0f) {
        spawnFalconEnemy();
        m_falconSpawned = true;
    }
}

void GameSession::spawnFalconEnemy() {
    if (!m_textures)
        return;

    PlayerEntity* player = getPlayer();
    if (!player)
        return;

    auto* playerTransform = player->getComponent<Transform>();
    if (!playerTransform)
        return;

    float spawnX = playerTransform->getPosition().x + WINDOW_WIDTH / 2.0f + 300.0f;

    auto enemy = std::make_unique<FalconEnemyEntity>(
        g_nextEntityId++,
        m_physicsManager.getWorld(),
        spawnX,
        0.0f,
        *m_textures);

    spawnEntity(std::move(enemy));
}