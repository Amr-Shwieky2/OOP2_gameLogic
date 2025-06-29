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

void GameSession::loadLevel(const std::string& levelPath) {
    // Clear existing entities
    m_entityManager.clear();
    m_player = nullptr;

    std::cout << "\n[GameSession] Loading level: " << levelPath << std::endl;

    // Load level from file
    bool success = m_levelLoader.loadFromFile(levelPath, m_entityManager, m_physicsWorld, *m_textures);

    if (success) {
        // Count entities by type
        int playerCount = 0, enemyCount = 0, coinCount = 0, giftCount = 0, otherCount = 0;

        // Find the player entity and count entity types
        for (auto* entity : m_entityManager.getAllEntities()) {
            if (auto* player = dynamic_cast<PlayerEntity*>(entity)) {
                m_player = player;
                playerCount++;
            }
            else if (dynamic_cast<EnemyEntity*>(entity)) {
                enemyCount++;
            }
            else if (dynamic_cast<CoinEntity*>(entity)) {
                coinCount++;
            }
            else if (dynamic_cast<GiftEntity*>(entity)) {
                giftCount++;
            }
            else {
                otherCount++;
            }
        }

        if (!m_player) {
            // No player in level file, create one
            std::cout << "[GameSession] No player in level, creating default player" << std::endl;
            auto playerEntity = EntityFactory::instance().create("Player", 200.0f, 400.0f);
            if (playerEntity) {
                m_player = dynamic_cast<PlayerEntity*>(playerEntity.get());
                m_entityManager.addEntity(std::move(playerEntity));
                playerCount = 1;
            }
        }

        std::cout << "[GameSession] Level loaded successfully!" << std::endl;
        std::cout << "  - Players: " << playerCount << std::endl;
        std::cout << "  - Enemies: " << enemyCount << std::endl;
        std::cout << "  - Coins: " << coinCount << std::endl;
        std::cout << "  - Gifts: " << giftCount << std::endl;
        std::cout << "  - Other: " << otherCount << std::endl;
        std::cout << "  - Total entities: " << m_entityManager.getAllEntities().size() << std::endl;
    }
    else {
        std::cerr << "[GameSession] Failed to load level: " << levelPath << std::endl;
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