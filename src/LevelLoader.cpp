#include "LevelLoader.h"
#include "GroundTile.h"
#include "Flag.h"
#include "Coin.h"
#include "SpeedGift.h"
#include "LifeHeartGift.h"
#include "ReverseMovementGift.h"
#include "ProtectiveShieldGift.h"
#include "HeadwindStormGift.h"
#include "RareCoinGift.h"
#include "MovableBox.h"
#include "SquareEnemy.h"
#include "Sea.h"
#include "MagneticGift.h"
#include "Constants.h"
#include <fstream>
#include <iostream>
#include <Cactus.h>

bool LevelLoader::loadFromFile(const std::string& path,
    GameObjectManager& objectManager,
    b2World& world,
    TextureManager& textures) {

    std::vector<std::string> lines = readLevelFile(path);
    if (lines.empty()) {
        std::cerr << "Error: Cannot read level file: " << path << "\n";
        return false;
    }

    objectManager.clear();

    int mapHeight = static_cast<int>(lines.size());

    for (int y = 0; y < mapHeight; ++y) {
        const std::string& row = lines[y];
        for (int x = 0; x < static_cast<int>(row.length()); ++x) {
            char tileChar = row[x];
            sf::Vector2f pos = calculatePosition(x, y);

            auto obj = createTileObject(tileChar, pos.x, pos.y, world, textures);
            if (obj) {
                objectManager.addGeneric(std::move(obj));
            }
        }
    }

    return true;
}

std::vector<std::string> LevelLoader::readLevelFile(const std::string& path) const {
    std::vector<std::string> lines;
    std::ifstream file(path);

    if (!file.is_open()) {
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

sf::Vector2f LevelLoader::calculatePosition(int x, int y) const {
    float posX = static_cast<float>(x * TILE_SIZE);
    float posY = WINDOW_HEIGHT - TILE_SIZE - static_cast<float>(y * TILE_SIZE);
    return sf::Vector2f(posX, posY);
}

std::unique_ptr<GameObject> LevelLoader::createTileObject(char tileChar, float x, float y,
    b2World& world, TextureManager& textures) {
    switch (tileChar) {
        // Ground tiles
    case 'G':
        return std::make_unique<GroundTile>(world, x, y, TileType::Ground, textures);
    case 'L':
        return std::make_unique<GroundTile>(world, x, y, TileType::Left, textures);
    case 'E':
        return std::make_unique<GroundTile>(world, x, y - TILE_SIZE, TileType::Edge, textures);
    case 'R':
        return std::make_unique<GroundTile>(world, x, y, TileType::Right, textures);
    case 'M':
        return std::make_unique<GroundTile>(world, x, y, TileType::Middle, textures);


        // Special objects
    case 'S':
        return std::make_unique<Sea>(x, y, textures);
    case 'B':
        return std::make_unique<MovableBox>(world, x, y, TileType::Box, textures);
    case 'X':
        return std::make_unique<Flag>(x + TILE_SIZE / 2.f, y - TILE_SIZE, textures);
    case 'c':
        return std::make_unique<Cactus>(x , y - TILE_SIZE  / 4.f, textures);


        // Collectibles
    case 'C':
        return std::make_unique<Coin>(x + TILE_SIZE / 4.f, y + TILE_SIZE / 4.f, textures);
    case 's':
        return std::make_unique<SpeedGift>(x, y, textures);
    case 'h':
        return std::make_unique<LifeHeartGift>(x, y, textures);
    case 'r':
        return std::make_unique<ReverseMovementGift>(x, y, textures);
    case 'p':
        return std::make_unique<ProtectiveShieldGift>(x, y, textures);
    case 'w':
        return std::make_unique<HeadwindStormGift>(x, y, textures);
    case '*':
        return std::make_unique<RareCoinGift>(x, y, textures);
    case 'm':
        return std::make_unique<MagneticGift>(x, y, textures);

        // Enemies
    case 'z':
        return std::make_unique<SquareEnemy>(world, x / PPM, y / PPM, textures);

    default:
        return nullptr;
    }
}

std::unique_ptr<GameObject> LevelLoader::createSpecialObject(char specialChar, float x, float y,
    TextureManager& textures) {
    switch (specialChar) {
    case 'C':
        return std::make_unique<Coin>(x + TILE_SIZE / 4.f, y + TILE_SIZE / 4.f, textures);
    case 's':
        return std::make_unique<SpeedGift>(x, y, textures);
    case 'h':
        return std::make_unique<LifeHeartGift>(x, y, textures);
    case 'r':
        return std::make_unique<ReverseMovementGift>(x, y, textures);
    case 'p':
        return std::make_unique<ProtectiveShieldGift>(x, y, textures);
    case 'w':
        return std::make_unique<HeadwindStormGift>(x, y, textures);
    case '*':
        return std::make_unique<RareCoinGift>(x, y, textures);
    case 'm':
        return std::make_unique<MagneticGift>(x, y, textures);
    default:
        return nullptr;
    }
}

std::unique_ptr<GameObject> LevelLoader::createEnemyObject(char enemyChar, float x, float y,
    b2World& world, TextureManager& textures) {
    switch (enemyChar) {
    case 'z':
        return std::make_unique<SquareEnemy>(world, x / PPM, y / PPM, textures);
    case 'Z':
        // ShootEnemy can be added here when implemented
        return nullptr;
    default:
        return nullptr;
    }
}

bool LevelLoader::isValidTileChar(char c) const {
    const std::string validChars = "GLERMSBX Cshpw*mzZ-";
    return validChars.find(c) != std::string::npos;
}

LevelLoader::LevelInfo LevelLoader::getLevelInfo(const std::string& path) const {
    LevelInfo info;
    info.name = path;
    info.description = "Level loaded from " + path;
    info.difficulty = 1;
    info.size = sf::Vector2i(0, 0);

    // Could be extended to read metadata from file header
    return info;
}