#pragma once

#include "Player.h"
#include "GameState.h"
#include "ICollectable.h"
#include "MovableBox.h"
#include "GroundTile.h"
#include <vector>
#include <memory>
#include <functional>

class GameObject;

class CollisionSystem {
private:
    Player& m_player;
    GameState& m_gameState;  
    std::function<void(std::unique_ptr<GameObject>)> m_spawnCallback;

public:
    CollisionSystem(Player& player, GameState& gameState,
        std::function<void(std::unique_ptr<GameObject>)> spawnCallback);

    // Main collision checking
    void checkCollisions(std::vector<std::unique_ptr<GameObject>>& objects);

private:
    // Collision handlers
    void handlePlayerCollectableCollision(ICollectable& collectable);
    void handlePlayerBoxCollision(MovableBox& box);
    void handlePlayerGroundCollision(GroundTile& ground);

    // Utility functions
    bool areColliding(const GameObject& obj1, const GameObject& obj2) const;
    sf::Vector2f calculatePushDirection(const GameObject& pusher, const GameObject& target) const;
};