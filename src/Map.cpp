#include "Map.h"

Map::Map(b2World& world, TextureManager& textures)
    : m_world(world), m_textures(textures) {

    // Initialize components
    m_objectManager = std::make_unique<GameObjectManager>();
    m_renderer = std::make_unique<MapRenderer>();
    m_levelLoader = std::make_unique<LevelLoader>();
}

bool Map::loadFromFile(const std::string& path) {
    m_currentLevelPath = path;

    bool success = m_levelLoader->loadFromFile(path, *m_objectManager, m_world, m_textures);
    if (success) {
        m_loaded = true;
        m_currentLevelInfo = m_levelLoader->getLevelInfo(path);
    }

    return success;
}

void Map::clear() {
    m_objectManager->clear();
    m_loaded = false;
    m_currentLevelPath.clear();
}

void Map::addStatic(std::unique_ptr<StaticGameObject> obj) {
    m_objectManager->addStatic(std::move(obj));
}

void Map::addDynamic(std::unique_ptr<DynamicGameObject> obj) {
    m_objectManager->addDynamic(std::move(obj));
}

void Map::addGeneric(std::unique_ptr<GameObject> obj) {
    m_objectManager->addGeneric(std::move(obj));
}

void Map::update(float deltaTime) {
    if (!m_loaded) return;

    // Update all dynamic objects
    m_objectManager->updateDynamicObjects(deltaTime);
}

void Map::render(sf::RenderTarget& target) const {
    if (!m_loaded) return;

    m_renderer->renderAll(target, m_objectManager->getAllObjects());
}

void Map::renderWithCulling(sf::RenderTarget& target, const sf::FloatRect& viewBounds) const {
    if (!m_loaded) return;

    m_renderer->renderWithCulling(target, m_objectManager->getAllObjects(), viewBounds);
}