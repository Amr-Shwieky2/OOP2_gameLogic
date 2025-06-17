#include "LevelLoader.h"
#include "Map.h"
#include "TileType.h"
#include "GroundTile.h"
#include "Flag.h"
#include "App.h"

// Gifts / collectibles
#include "Coin.h"
#include "LifeHeartGift.h"
#include "SpeedGift.h"
#include "HeadwindStormGift.h"
#include "ReverseMovementGift.h"
#include "ProtectiveShieldGift.h"
#include "CloseBox.h"
#include "RareCoinGift.h"

#include <fstream>
#include <iostream>
#include <memory>

bool LevelLoader::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    m_levelData.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::vector<TileType> row;
        for (char c : line) {
            row.push_back(charToTileType(c));
        }
        m_levelData.push_back(row);
    }
    return true;
}

void LevelLoader::loadLevel(const std::string& path, Map& map, b2World& world, TextureManager& textures) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << path << std::endl;
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    const int mapHeight = static_cast<int>(lines.size());

    for (int y = 0; y < mapHeight; ++y) {
        const std::string& row = lines[y];

        for (int x = 0; x < row.length(); ++x) {
            float posX = static_cast<float>(x * TILE_SIZE);
            float posY = WINDOW_HEIGHT - TILE_SIZE - static_cast<float>(y * TILE_SIZE);
            char symbol = row[x];

            switch (symbol) {
            case 'M':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Middle, textures));
                break;
            case 'L':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Left, textures));
                break;
            case 'F':
                posY -= TILE_SIZE;
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::LeftEdge, textures));
                break;
            case 'R':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Right, textures));
                break;
            case 'E':
                posY -= TILE_SIZE;
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::RightEdge, textures));
                break;
            case 'G':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Ground, textures));
                break;
            case 'S':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Sea, textures));
                break;
            case 'X': {
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Middle, textures));
                map.addTile(std::make_unique<Flag>(posX + TILE_SIZE / 2.f, posY - TILE_SIZE, textures));
                break;
            }
            case 'C': // Coin
                map.addGameObject(std::make_unique<Coin>(posX + TILE_SIZE / 4.f, posY + TILE_SIZE / 4.f, textures));
                break;
            case 'H': // Life Heart
                map.addGameObject(std::make_unique<LifeHeartGift>(posX + TILE_SIZE / 4.f, posY + TILE_SIZE / 4.f, textures));
                break;
            case 's': // Speed Gift
                map.addGameObject(std::make_unique<SpeedGift>(posX + TILE_SIZE / 4.f, posY + TILE_SIZE / 4.f, textures));
                break;
            case 'B': // Reverse Movement Gift
                map.addGameObject(std::make_unique<ReverseMovementGift>(
                    posX + TILE_SIZE / 4.f, posY + TILE_SIZE / 4.f, textures));
                break;

            case 'P': // Protective Shield Gift
                map.addGameObject(std::make_unique<ProtectiveShieldGift>(
                    posX + TILE_SIZE / 4.f, posY + TILE_SIZE / 4.f, textures));
                break;

            case 'W': // Headwind Storm Gift
                map.addGameObject(std::make_unique<HeadwindStormGift>(
                    posX + TILE_SIZE / 4.f, posY + TILE_SIZE / 4.f, textures));
                break;

            case 'O': // CloseBox
                map.addGameObject(std::make_unique<CloseBox>(
                    posX + TILE_SIZE / 4.f, posY + TILE_SIZE / 4.f, textures));
                break;

            case 'Z': // RareCoinGift (optional: use mostly via CloseBox spawn)
                map.addGameObject(std::make_unique<RareCoinGift>(
                    posX + TILE_SIZE / 4.f, posY + TILE_SIZE / 4.f, textures));
                break;

                // TODO: Add cases like CloseBox, ReverseGift, Shield, etc.
            case '-':
            default:
                break; // skip unknown symbols
            }
        }
    }
}

const std::vector<std::vector<TileType>>& LevelLoader::getLevelData() const {
    return m_levelData;
}

TileType LevelLoader::charToTileType(char c) const {
    switch (c) {
    case 'M': return TileType::Middle;
    case 'R': return TileType::Right;
    case 'r': return TileType::RightEdge;
    case 'L': return TileType::Left;
    case 'l': return TileType::LeftEdge;
    case 'S': return TileType::Sea;
    case '-': return TileType::Empty;
    case 'G': return TileType::Ground;
    case 'X': return TileType::Flag;
    default:  return TileType::Empty;
    }
}
