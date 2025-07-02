#include "GameSession.h"
#include "GameCollisionSetup.h"
#include "EntityFactory.h"
#include "LevelLoader.h"
#include "PlayerEntity.h"
#include "EnemyEntity.h"
#include "AIComponent.h"
#include "CollisionComponent.h"
#include "Transform.h"
#include "Constants.h"
#include <iostream>
#include <RenderComponent.h>
#include <PhysicsComponent.h>
#include <FlagEntity.h>
#include <cmath> 

// Global pointer to the current active session
GameSession* g_currentSession = nullptr;

GameSession::GameSession()
    : m_physicsWorld(b2Vec2(0.0f, 9.8f)) {
    g_currentSession = this;
}

GameSession::~GameSession() {
    if (g_currentSession == this) {
        g_currentSession = nullptr;
    }
}

void GameSession::initialize(TextureManager& textures, sf::RenderWindow& window) {
    m_textures = &textures;

    // Register all entity types
    registerGameEntities(m_physicsWorld, textures);

    // Setup collision handlers
    setupGameCollisionHandlers(m_collisionSystem);
    setupEventHandlers();

    // Initialize surprise box manager - FIX: pass window reference properly
    m_surpriseBoxManager = std::make_unique<SurpriseBoxManager>(textures, window);
    m_surpriseBoxManager->setEntityManager(&m_entityManager);
    m_surpriseBoxManager->setPhysicsWorld(&m_physicsWorld);

    std::cout << "[GameSession] SurpriseBoxManager initialized with window" << std::endl;
}

void GameSession::loadLevel(const std::string& levelPath) {
    std::cout << "\n[GameSession] Loading level: " << levelPath << std::endl;

    // Clear existing entities 
    m_entityManager.clear();
    m_player = nullptr;
    if (m_surpriseBoxManager) {
        m_surpriseBoxManager->reset(); 
    }

    m_levelTransitionPending = false;
    m_transitionTimer = 0.0f;
    m_needLevelSwitch = false;

    // Load level from file
    bool success = m_levelLoader.loadFromFile(levelPath, m_entityManager, m_physicsWorld, *m_textures);

    if (success) {
        // Find the player entity
        for (auto* entity : m_entityManager.getAllEntities()) {
            if (auto* player = dynamic_cast<PlayerEntity*>(entity)) {
                m_player = player;
                break;
            }
        }

        if (!m_player) {
            std::cout << "[GameSession] No player in level, creating default player" << std::endl;
            auto playerEntity = EntityFactory::instance().create("Player", 200.0f, 400.0f);
            if (playerEntity) {
                m_player = dynamic_cast<PlayerEntity*>(playerEntity.get());
                m_entityManager.addEntity(std::move(playerEntity));
            }
        }

        if (m_player && m_surpriseBoxManager) {
            m_surpriseBoxManager->setPlayer(m_player);
            m_surpriseBoxManager->setEntityManager(&m_entityManager);
            m_surpriseBoxManager->setPhysicsWorld(&m_physicsWorld);
        }

        std::cout << "[GameSession] Level loaded successfully: " << levelPath << std::endl;
    }
    else {
        std::cerr << "[GameSession] Failed to load level: " << levelPath << std::endl;
    }
}


void GameSession::update(float deltaTime) {
    if (m_needLevelSwitch) {
        std::cout << "[GameSession] Performing delayed level switch to: " << m_nextLevelPath << std::endl;
        m_needLevelSwitch = false;
        loadLevel(m_nextLevelPath);
        return;
    }
    if (m_levelTransitionPending) {
        m_transitionTimer += deltaTime;

        std::cout << "[DEBUG] Transition timer: " << m_transitionTimer
            << "/" << m_transitionDelay << std::endl;

        if (m_transitionTimer >= m_transitionDelay) {
            m_levelTransitionPending = false;

            std::cout << "[GameSession] Starting level transition..." << std::endl;

            if (!loadNextLevel()) {
                std::cout << "[GameSession] All levels completed!" << std::endl;
            }

            return;
        }
    }

    // Update physics
    m_physicsWorld.Step(deltaTime, 8, 3);

    // Update all entities
    m_entityManager.updateAll(deltaTime);

    // Check collisions
    checkCollisions();
}

void GameSession::render(sf::RenderWindow& window) {
    // Render all entities
    m_renderSystem.render(m_entityManager, window);
}

// FIXED: Use addEntity instead of createEntity with unique_ptr
void GameSession::spawnEntity(std::unique_ptr<Entity> entity) {
    if (entity) {
        Entity* ptr = entity.get();  // Get pointer before moving
        m_entityManager.addEntity(std::move(entity));

        // If it's a player, store reference
        if (auto* player = dynamic_cast<PlayerEntity*>(ptr)) {
            m_player = player;
        }
    }
}

void GameSession::checkCollisions() {
    auto entities = m_entityManager.getAllEntities();

    for (size_t i = 0; i < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
            if (entities[i]->isActive() && entities[j]->isActive()) {
                // إضافة debug للعلم
                bool isPlayerFlag = (dynamic_cast<PlayerEntity*>(entities[i]) && dynamic_cast<FlagEntity*>(entities[j])) ||
                    (dynamic_cast<FlagEntity*>(entities[i]) && dynamic_cast<PlayerEntity*>(entities[j]));

                if (areColliding(*entities[i], *entities[j])) {
                    if (isPlayerFlag) {
                        std::cout << "[DEBUG] Player-Flag collision detected!" << std::endl;
                    }
                    m_collisionSystem.processCollision(*entities[i], *entities[j]);
                }
            }
        }
    }
}

bool GameSession::areColliding(Entity& a, Entity& b) {
    auto* collA = a.getComponent<CollisionComponent>();
    auto* collB = b.getComponent<CollisionComponent>();

    if (!collA || !collB) return false;

    if (!a.isActive() || !b.isActive()) return false;

    if ((collA->getLayer() & collB->getMask()) == 0) return false;

    // Get positions
    auto* transA = a.getComponent<Transform>();
    auto* transB = b.getComponent<Transform>();

    if (!transA || !transB) return false;

    // Simple distance check
    sf::Vector2f posA = transA->getPosition();
    sf::Vector2f posB = transB->getPosition();

    float dx = posA.x - posB.x;
    float dy = posA.y - posB.y;
    float distSq = dx * dx + dy * dy;

    bool isPlayerFlag = (dynamic_cast<PlayerEntity*>(&a) && dynamic_cast<FlagEntity*>(&b)) ||
        (dynamic_cast<FlagEntity*>(&a) && dynamic_cast<PlayerEntity*>(&b));

    if (isPlayerFlag) {
        float distance = std::sqrt(distSq);
        return distSq < (150.0f * 150.0f); 
    }

    return distSq < (80.0f * 80.0f);
}

void GameSession::setupEventHandlers() {
    EventSystem::getInstance().subscribe<FlagReachedEvent>(
        [this](const FlagReachedEvent& event) {
            this->onFlagReached(event);
        }
    );

    EventSystem::getInstance().subscribe<LevelTransitionEvent>(
        [this](const LevelTransitionEvent& event) {
            this->onLevelTransition(event);
        }
    );
}

void GameSession::onFlagReached(const FlagReachedEvent& event) {
    std::cout << "[GameSession] Flag reached! Starting level transition..." << std::endl;

    m_levelTransitionPending = true;
    m_transitionTimer = 0.0f;
}

void GameSession::onLevelTransition(const LevelTransitionEvent& event) {
    if (event.isGameComplete) {
        std::cout << "[GameSession] Game completed! All levels finished!" << std::endl;
    }
    else {
        std::cout << "[GameSession] Transitioning from " << event.fromLevel
            << " to " << event.toLevel << std::endl;
    }
}

bool GameSession::loadNextLevel() {
    if (m_levelManager.hasNextLevel()) {
        std::string currentLevel = m_levelManager.getCurrentLevelPath();

        if (m_levelManager.loadNextLevel()) {
            std::string nextLevel = m_levelManager.getCurrentLevelPath();

            m_nextLevelPath = nextLevel;
            m_needLevelSwitch = true;

            std::cout << "[GameSession] Next level queued: " << nextLevel << std::endl;

            EventSystem::getInstance().publish(
                LevelTransitionEvent(currentLevel, nextLevel, false)
            );

            return true;
        }
    }
    else {
        EventSystem::getInstance().publish(
            LevelTransitionEvent(m_levelManager.getCurrentLevelPath(), "", true)
        );

        std::cout << "[GameSession] Game Complete! No more levels!" << std::endl;
        return false;
    }

    return false;
}

void GameSession::reloadCurrentLevel() {
    std::string currentLevel = m_levelManager.getCurrentLevelPath();
    std::cout << "[GameSession] Reloading current level: " << currentLevel << std::endl;
    loadLevel(currentLevel);
}

const std::string& GameSession::getCurrentLevelName() const {
    return m_levelManager.getCurrentLevelPath();
}