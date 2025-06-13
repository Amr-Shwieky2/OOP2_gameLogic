#include "Map.h"
#include "LevelLoader.h"

Map::Map(b2World& world, TextureManager& textures)
    : m_world(world), m_textures(textures) {
}

void Map::clear() {
    m_tiles.clear();
}

void Map::addTile(std::unique_ptr<Tile> tile) {
    m_tiles.push_back(std::move(tile));
}

void Map::render(sf::RenderTarget& target) const {
    for (const auto& tile : m_tiles) {
        tile->render(target);
    }
}

void Map::loadFromFile(const std::string& path) {
    clear(); // Clear existing tiles before loading new ones
    LevelLoader::loadLevel(path, *this, m_world, m_textures);
}
