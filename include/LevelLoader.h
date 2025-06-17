#pragma once

#include <vector>
#include <string>
#include "TileType.h"
#include "Map.h"            
#include "ResourceManager.h"



class LevelLoader {
public:
    static void loadLevel(const std::string& path, Map& map, b2World& world, TextureManager& textures);

    bool loadFromFile(const std::string& filename);
    const std::vector<std::vector<TileType>>& getLevelData() const;

private:
    std::vector<std::vector<TileType>> m_levelData;

    TileType charToTileType(char c) const;
};
