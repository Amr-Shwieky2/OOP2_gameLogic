#include "LevelLoader.h"
#include "Map.h"
#include "TileType.h"
#include "GroundTile.h"
#include "Flag.h"
#include "ResourceManager.h"
#include <fstream>
#include <iostream>
#include "App.h"

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

    // Load all lines into memory to determine total height
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
            float posY = WINDOW_HEIGHT - TILE_SIZE - static_cast<float>(y * TILE_SIZE);  // Align to bottom

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
                // Flag always placed above a middle tile
                map.addTile(std::make_unique<GroundTile>(world, posX, posY, TileType::Middle, textures));
                map.addTile(std::make_unique<Flag>(posX + TILE_SIZE / 2.f, posY - TILE_SIZE, textures));
                break;
            }
            case '-':
            default:
                break; // Do nothing for empty/unknown tiles
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
