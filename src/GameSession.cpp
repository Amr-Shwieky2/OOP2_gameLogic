#include "GameSession.h"
#include "PlayerEntity.h"
#include "GameCollisionSetup.h"
#include "ResourceManager.h"
#include <iostream>

// Global pointer for backwards compatibility
GameSession* g_currentSession = nullptr;

GameSession::GameSession() {
    g_currentSession = this;
    std::cout << "[GameSession] Created - SRP compliant!" << std::endl;
}

GameSession::~GameSession() {
    std::cout << "[GameSession] Starting destruction..." << std::endl;

    // FIRST: Clear global pointer to prevent any access
    if (g_currentSession == this) {
        std::cout << "[GameSession] Clearing global session pointer" << std::endl;
        g_currentSession = nullptr;
    }

    try {
        // Clear player cache immediately
        std::cout << "[GameSession] Clearing player cache" << std::endl;
        m_player = nullptr;

        // Shutdown surprise box manager FIRST (it has event subscriptions)
        std::cout << "[GameSession] Destroying surprise box manager..." << std::endl;
        if (m_surpriseBoxManager) {
            m_surpriseBoxManager->reset();
            m_surpriseBoxManager.reset();
        }

        // Clear all event handlers
        std::cout << "[GameSession] Shutting down event coordinator..." << std::endl;
        m_eventCoordinator.shutdown();

        // Clear collision system
        std::cout << "[GameSession] Clearing collision handlers..." << std::endl;
        m_collisionManager.clearHandlers();

        // Force cleanup of entities
        std::cout << "[GameSession] Force cleaning entities..." << std::endl;
        m_cleanupManager.forceCleanup(m_entityManager);

        // Clear all entities
        std::cout << "[GameSession] Clearing entity manager..." << std::endl;
        m_entityManager.clear();

        // Clear any remaining event system listeners
        std::cout << "[GameSession] Clearing event system..." << std::endl;
        EventSystem::getInstance().clear();

        std::cout << "[GameSession] Destruction complete successfully" << std::endl;

    }
    catch (const std::exception& e) {
        std::cout << "[GameSession] Exception during destruction: " << e.what() << std::endl;
    }
}

void GameSession::initialize(TextureManager& textures, sf::RenderWindow& window) {
    m_textures = &textures;

    std::cout << "[GameSession] Initializing all managers..." << std::endl;

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

    std::cout << "[GameSession] All managers initialized successfully!" << std::endl;
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
    // Delegate to level manager
    return m_levelManager.loadLevel(levelPath);
}

bool GameSession::loadNextLevel() {
    m_player = nullptr; // Reset player cache
    // Delegate to level manager
    return m_levelManager.loadNextLevel();
}

void GameSession::reloadCurrentLevel() {
    m_player = nullptr; // Reset player cache
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

    // 3. Update all entities
    m_entityManager.updateAll(deltaTime);

    // 4. Check collisions
    m_collisionManager.checkCollisions(m_entityManager);

    // 5. Cleanup inactive entities
    m_cleanupManager.update(deltaTime);
    m_cleanupManager.cleanupInactiveEntities(m_entityManager);
}