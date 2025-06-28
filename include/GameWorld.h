#pragma once
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include "Player.h"
#include "Map.h"
#include "ResourceManager.h"
#include "CollisionSystem.h"

class GameWorld {
public:
    GameWorld();
    ~GameWorld();

    void initialize(TextureManager& textures);
    void loadLevel(const std::string& levelPath);
    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    // Getters
    Player* getPlayer() const { return m_player.get(); }
    Map* getMap() const { return m_map.get(); }
    b2World& getPhysicsWorld() { return m_world; }

    // Object spawning
    void spawnGameObject(std::unique_ptr<GameObject> obj);

private:
    b2World m_world;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Map> m_map;
    //std::unique_ptr<CollisionSystem> m_collisionSystem;
    TextureManager* m_textures = nullptr;
};