#pragma once

#include <vector>
#include <memory>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include "GroundTile.h"
#include "Flag.h"
#include "ResourceManager.h"
#include <LifeGift.h>

class LevelLoader; // Forward declaration

class Map {
public:
    Map(b2World& world, TextureManager& textures);

    // Load map from level file
    void loadFromFile(const std::string& path);

    // Clear current tiles
    void clear();

    // Add a new tile (used by loader)
    void addTile(std::unique_ptr<Tile> tile);

    // Draw map to the screen
    void render(sf::RenderTarget& target) const;

    void addCollectible(std::unique_ptr<Collectible> c);
    std::vector<std::unique_ptr<Collectible>>& getCollectibles();

private:
    std::vector<std::unique_ptr<Tile>> m_tiles;
    std::vector<std::unique_ptr<Collectible>> m_collectibles;

    b2World& m_world;
    TextureManager& m_textures;
};
