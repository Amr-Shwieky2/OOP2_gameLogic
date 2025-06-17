#pragma once

#include <vector>
#include <memory>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include "GroundTile.h"
#include "Flag.h"
#include "ResourceManager.h"
#include "IUpdatable.h"       // <-- Add this
#include "GameObject.h"       // Needed for all dynamic items

class LevelLoader; // Forward declaration

class Map {
public:
    Map(b2World& world, TextureManager& textures);

    void loadFromFile(const std::string& path);
    void clear();

    void addTile(std::unique_ptr<Tile> tile);
    void render(sf::RenderTarget& target) const;

    void addGameObject(std::unique_ptr<GameObject> obj);
    void addUpdatable(IUpdatable* updatable);

    void update(float deltaTime);

    std::vector<std::unique_ptr<GameObject>>& getGameObjects();


private:
    std::vector<std::unique_ptr<Tile>> m_tiles;
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    std::vector<IUpdatable*> m_updatables;

    b2World& m_world;
    TextureManager& m_textures;
};
