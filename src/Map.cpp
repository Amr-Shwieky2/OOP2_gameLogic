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
    for (const auto& c : m_collectibles)
        c->render(target);
}

void Map::addCollectible(std::unique_ptr<Collectible> c)
{
	m_collectibles.push_back(std::move(c));
}

std::vector<std::unique_ptr<Collectible>>& Map::getCollectibles()
{
    return m_collectibles;
}

void Map::loadFromFile(const std::string& path) {
    clear(); // Clear existing tiles before loading new ones
    LevelLoader::loadLevel(path, *this, m_world, m_textures);
}
