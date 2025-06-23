#pragma once
#include <string>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include <box2d/b2_body.h>

class Map;
class Player;

class LevelLoader {
public:
    static bool loadFromFile(const std::string& path, Map& map, b2World& world, ResourceManager<sf::Texture>& textures);
};
