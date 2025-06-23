#include "LevelLoader.h"
#include "Map.h"
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

#include <fstream>
#include <iostream>


bool LevelLoader::loadFromFile(const std::string& path, Map& map, b2World& world, ResourceManager<sf::Texture>& textures)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open level file: " << path << "\n";
        return false;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
        lines.push_back(line);

    int mapHeight = static_cast<int>(lines.size());

    for (int y = 0; y < mapHeight; ++y) {
        const std::string& row = lines[y];
        for (int x = 0; x < static_cast<int>(row.length()); ++x) {
            float posX = static_cast<float>(x * TILE_SIZE);
            float posY = WINDOW_HEIGHT - TILE_SIZE - static_cast<float>(y * TILE_SIZE);
            char tileChar = row[x];

            switch (tileChar) {
            case 'G':
                map.addStatic(std::make_unique<GroundTile>(world, posX, posY, TileType::Ground, textures));
                break;
            case 'L':
                map.addStatic(std::make_unique<GroundTile>(world, posX, posY, TileType::Left, textures));
                break;
            case 'E':
                posY -= TILE_SIZE;
                map.addStatic(std::make_unique<GroundTile>(world, posX, posY, TileType::Edge, textures));
                break;
            case 'R':
                map.addStatic(std::make_unique<GroundTile>(world, posX, posY, TileType::Right, textures));
                break;
            case 'M':
                map.addStatic(std::make_unique<GroundTile>(world, posX, posY, TileType::Middle, textures));
                break;
            case 'S':
                map.addStatic(std::make_unique<GroundTile>(world, posX, posY, TileType::Sea, textures));
                break;
            case 'B':
                map.addDynamic(std::make_unique<MovableBox>(world, posX, posY, TileType::Box, textures));
                break;
            case 'z': // SquareEnemy
                map.addDynamic(std::make_unique<SquareEnemy>(world, posX / PPM, posY / PPM, textures));
                break;
            case 'Z': // ShootEnemy
                //map.addDynamic(std::make_unique<ShootEnemy>(world, posX / PPM, posY / PPM, textures, player));
                break;
            case 'X':
                map.addStatic(std::make_unique<Flag>(posX + TILE_SIZE / 2.f, posY - TILE_SIZE, textures));
                break;
            case 'C':
                map.addDynamic(std::make_unique<Coin>(posX + TILE_SIZE / 4.f, posY + TILE_SIZE / 4.f, textures));
                break;
            case 's':
                map.addStatic(std::make_unique<SpeedGift>(posX, posY, textures));
                break;
            case 'h':
                map.addStatic(std::make_unique<LifeHeartGift>(posX, posY, textures));
                break;
            case 'r':
                map.addStatic(std::make_unique<ReverseMovementGift>(posX, posY, textures));
                break;
            case 'p':
                map.addStatic(std::make_unique<ProtectiveShieldGift>(posX, posY, textures));
                break;
            case 'w':
                map.addStatic(std::make_unique<HeadwindStormGift>(posX, posY, textures));
                break;
            case '*':
                map.addStatic(std::make_unique<RareCoinGift>(posX, posY, textures));
                break;
            default:
                break;
            }
        }
    }

    return true;
}
