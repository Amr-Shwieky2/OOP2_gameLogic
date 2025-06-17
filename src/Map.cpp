#include "Map.h"
#include "LevelLoader.h"

Map::Map(b2World& world, TextureManager& textures)
    : m_world(world), m_textures(textures) {
}

void Map::clear() {
    m_tiles.clear();
    m_gameObjects.clear();
    m_updatables.clear();
}

void Map::loadFromFile(const std::string& path) {
    clear();
    LevelLoader::loadLevel(path, *this, m_world, m_textures);
}

void Map::addTile(std::unique_ptr<Tile> tile) {
    m_tiles.push_back(std::move(tile));
}

void Map::addGameObject(std::unique_ptr<GameObject> obj) {
    // Check if it's also updatable
    if (auto updatable = dynamic_cast<IUpdatable*>(obj.get())) {
        m_updatables.push_back(updatable);
    }
    m_gameObjects.push_back(std::move(obj));
}

void Map::addUpdatable(IUpdatable* updatable) {
    m_updatables.push_back(updatable); // In case something is added manually
}

void Map::update(float deltaTime) {
    for (auto* obj : m_updatables)
        obj->update(deltaTime);
}

void Map::render(sf::RenderTarget& target) const {
    for (const auto& tile : m_tiles)
        tile->render(target);

    for (const auto& obj : m_gameObjects)
        obj->render(target);
}

std::vector<std::unique_ptr<GameObject>>& Map::getGameObjects() {
    return m_gameObjects;
}



