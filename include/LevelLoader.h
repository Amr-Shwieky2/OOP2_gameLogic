#pragma once

#include <vector>
#include <string>
#include "TileType.h"
#include "Map.h"
#include "ResourceManager.h"


class LevelLoader {
public:
    // Loads a level from file and populates the given map
    static void loadLevel(const std::string& path, Map& map, b2World& world, TextureManager& textures);

    // Optional: loads and stores level data in memory (not required for rendering)
    bool loadFromFile(const std::string& filename);

    // Access raw level data if needed
    const std::vector<std::vector<TileType>>& getLevelData() const;

private:
    std::vector<std::vector<TileType>> m_levelData;

    // Utility: converts a character from the file into a TileType
    TileType charToTileType(char c) const;
};
