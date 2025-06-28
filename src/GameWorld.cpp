#include "GameWorld.h"
#include "Constants.h"

GameWorld::GameWorld() : m_world(b2Vec2(0.f, 9.8f)) {}

GameWorld::~GameWorld() = default;

void GameWorld::initialize(TextureManager& textures) {
    m_textures = &textures;
    m_map = std::make_unique<Map>(m_world, textures);

    // Create collision system with spawn callback
    /*m_collisionSystem = std::make_unique<CollisionSystem>(
        *m_player,
        [this](std::unique_ptr<GameObject> obj) { spawnGameObject(std::move(obj)); }
    );*/
}

void GameWorld::loadLevel(const std::string& levelPath) {
    if (!m_map || !m_textures) return;

    m_map->loadFromFile(levelPath);
    m_player = std::make_unique<Player>(m_world, 128.f / PPM, 600.f / PPM, *m_textures);

    // Recreate collision system with new player
    /*m_collisionSystem = std::make_unique<CollisionSystem>(
        *m_player,
        [this](std::unique_ptr<GameObject> obj) { spawnGameObject(std::move(obj)); }
    );*/
}

void GameWorld::update(float deltaTime) {
    // Step physics
    m_world.Step(deltaTime, 8, 3);

    // Update game objects
    if (m_map) {
        m_map->update(deltaTime);
    }

    if (m_player) {
        m_player->update(deltaTime);
    }

    // Handle collisions
    if (  m_map) {
        //m_collisionSystem->checkCollisions(m_map->getObjects());m_collisionSystem&&
    }
}

void GameWorld::render(sf::RenderTarget& target) const {
    if (m_map) {
        m_map->render(target);
    }

    if (m_player) {
        m_player->render(target);
    }
}

void GameWorld::spawnGameObject(std::unique_ptr<GameObject> obj) {
    if (!obj || !m_map) return;

    // استخدم addGeneric - هو سيتولى التصنيف تلقائياً
    m_map->addGeneric(std::move(obj));
}
