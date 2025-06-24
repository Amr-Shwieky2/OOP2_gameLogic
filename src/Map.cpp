#include "Map.h"
#include "LevelLoader.h"

Map::Map(b2World& world, TextureManager& textures)
    : m_world(world), m_textures(textures){
}

void Map::loadFromFile(const std::string& path) {
    clear();
    LevelLoader::loadFromFile(path, *this, m_world, m_textures);
}

void Map::clear() {
    m_gameObjects.clear();
    m_updatables.clear();
}

void Map::addStatic(std::unique_ptr<GameObject> obj) {
    m_gameObjects.push_back(std::move(obj));
}

void Map::addDynamic(std::unique_ptr<DynamicGameObject> obj) {
    m_updatables.push_back(obj.get());
    m_gameObjects.push_back(std::move(obj));
}

void Map::update(float deltaTime) {
    for (auto& obj : m_gameObjects) {
        obj->update(deltaTime);
    }
}

void Map::render(sf::RenderTarget& target) const {
    for (const auto& obj : m_gameObjects)
        obj->render(target);
}

std::vector<std::unique_ptr<GameObject>>& Map::getObjects() {
    return m_gameObjects;
}
