#include "GameSession.h"
#include "GameCollisionSetup.h"
#include "EntityFactory.h"
#include "LevelLoader.h"
#include "PlayerEntity.h"
#include "CollisionComponent.h"
#include "Transform.h"
#include "Constants.h"
#include <iostream>

GameSession::GameSession()
    : m_physicsWorld(b2Vec2(0.0f, 9.8f)) {
}

GameSession::~GameSession() = default;

void GameSession::initialize(TextureManager& textures) {
    m_textures = &textures;

    // Register all entity types
    registerGameEntities(m_physicsWorld, textures);

    // Setup collision handlers
    setupGameCollisionHandlers(m_collisionSystem);
}

// Update loadLevel in GameSession.cpp:
void GameSession::loadLevel(const std::string& levelPath) {
    // Clear existing entities
    m_entityManager.clear();
    m_player = nullptr;

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
            // No player in level file, create one
            auto playerEntity = EntityFactory::instance().create("Player", 200.0f, 400.0f);
            if (playerEntity) {
                m_player = dynamic_cast<PlayerEntity*>(playerEntity.get());
                m_entityManager.addEntity(std::move(playerEntity));
            }
        }

        std::cout << "Level loaded successfully: " << levelPath << std::endl;
    }
    else {
        std::cerr << "Failed to load level: " << levelPath << std::endl;
    }
}
void GameSession::update(float deltaTime) {
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

void GameSession::spawnEntity(std::unique_ptr<Entity> entity) {
    if (entity) {
        m_entityManager.createEntity<Entity>(std::move(entity));
    }
}

void GameSession::checkCollisions() {
    auto entities = m_entityManager.getAllEntities();

    for (size_t i = 0; i < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
            if (entities[i]->isActive() && entities[j]->isActive()) {
                if (areColliding(*entities[i], *entities[j])) {
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

    // Check layer mask
    if ((collA->getLayer() & collB->getMask()) == 0) return false;

    // Get positions
    auto* transA = a.getComponent<Transform>();
    auto* transB = b.getComponent<Transform>();

    if (!transA || !transB) return false;

    // Simple distance check (you can improve this with proper bounds)
    sf::Vector2f posA = transA->getPosition();
    sf::Vector2f posB = transB->getPosition();

    float dx = posA.x - posB.x;
    float dy = posA.y - posB.y;
    float distSq = dx * dx + dy * dy;

    return distSq < (50.0f * 50.0f); // 50 pixel collision radius
}