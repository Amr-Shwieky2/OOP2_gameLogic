#pragma once
#include <string>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "ResourceManager.h"
#include "GameObjectManager.h"

class LevelLoader {
public:
    LevelLoader() = default;

    // Load level from file
    bool loadFromFile(const std::string& path,
        GameObjectManager& objectManager,
        b2World& world,
        TextureManager& textures);

    // Get level metadata
    struct LevelInfo {
        std::string name;
        std::string description;
        int difficulty;
        sf::Vector2i size;
    };

    LevelInfo getLevelInfo(const std::string& path) const;

private:
    // Object creation helpers
    std::unique_ptr<GameObject> createTileObject(char tileChar, float x, float y,
        b2World& world, TextureManager& textures);
    std::unique_ptr<GameObject> createSpecialObject(char specialChar, float x, float y,
        TextureManager& textures);
    std::unique_ptr<GameObject> createEnemyObject(char enemyChar, float x, float y,
        b2World& world, TextureManager& textures);

    // Parsing helpers
    std::vector<std::string> readLevelFile(const std::string& path) const;
    bool isValidTileChar(char c) const;
    sf::Vector2f calculatePosition(int x, int y) const;
};
