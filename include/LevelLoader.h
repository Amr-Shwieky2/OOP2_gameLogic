#pragma once

#include <string>
#include <Box2D/Box2D.h>
#include "ResourceManager.h"

class Map;

class LevelLoader {
public:
    static bool loadFromFile(const std::string& path, Map& map, b2World& world, TextureManager& textures);
};
