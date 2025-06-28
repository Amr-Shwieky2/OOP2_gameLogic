#pragma once
#include <string>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Entity.h"
#include "ResourceManager.h"

class EntityManager;
class EntityFactory;

class LevelLoader {
public:
    LevelLoader() = default;

    // Updated load method to use EntityManager instead of GameObjectManager
    bool loadFromFile(const std::string& path,
        EntityManager& entityManager,
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
    // Create entity based on character
    std::unique_ptr<Entity> createEntityForChar(char tileChar, float x, float y,
        b2World& world, TextureManager& textures);

    // Parsing helpers
    std::vector<std::string> readLevelFile(const std::string& path) const;
    bool isValidTileChar(char c) const;
    sf::Vector2f calculatePosition(int x, int y) const;
};