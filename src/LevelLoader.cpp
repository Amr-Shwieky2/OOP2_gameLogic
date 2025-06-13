#include "LevelLoader.h"
#include "Map.h"
#include "TileType.h"
#include "GroundTile.h"
#include "Flag.h"
#include "ResourceManager.h"
#include <fstream>
#include <iostream>

bool LevelLoader::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

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

    std::string line;
    int y = 0;

    while (std::getline(file, line)) {
        for (int x = 0; x < line.length(); ++x) {
            float posX = static_cast<float>(x * TILE_SIZE);
            float posY = static_cast<float>(y * TILE_SIZE);
            char symbol = line[x];

            // Place tiles based on character
            switch (symbol) {
            case 'M':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Middle, textures));
                break;
            case 'L':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Left, textures));
                break;
            case 'l':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::LeftEdge, textures));
                break;
            case 'R':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Right, textures));
                break;
            case 'r':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::RightEdge, textures));
                break;
            case 'G':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Ground, textures));
                break;
            case 'S':
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Sea, textures));
                break;
            case 'X': {
                // Flag must be placed over a ground tile
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Middle, textures));
                map.addTile(std::make_unique<Flag>(posX + TILE_SIZE / 2.f, posY - TILE_SIZE, textures));
                break;
            }
            case '-':
            default:
                break; // Empty or unrecognized tile
            }
        }
        ++y;
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
