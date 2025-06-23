#pragma once

#include <vector>
#include <memory>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

#include "ResourceManager.h"
#include "GameObject.h"
#include "DynamicGameObject.h"

class Player;

class Map {
public:

    Map(b2World& world, TextureManager& textures);

    void loadFromFile(const std::string& path);
    void clear();

    // Unified adders
    void addStatic(std::unique_ptr<GameObject> obj);
    void addDynamic(std::unique_ptr<DynamicGameObject> obj);

    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    std::vector<std::unique_ptr<GameObject>>& getObjects();

private:
    b2World& m_world;
    TextureManager& m_textures;

    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    std::vector<DynamicGameObject*> m_updatables;
};
