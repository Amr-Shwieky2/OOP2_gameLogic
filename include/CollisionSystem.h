#pragma once

#include "MultiMethodCollisionHandler.h"
#include "Player.h"
#include "Coin.h"
#include "LifeHeartGift.h"
#include "SpeedGift.h"
#include "ReverseMovementGift.h"
#include "ProtectiveShieldGift.h"
#include "HeadwindStormGift.h"
#include "RareCoinGift.h"
#include "MovableBox.h"
#include <vector>
#include <memory>
#include <functional>

class CollisionSystem {
public:
    CollisionSystem(Player& player, std::function<void(std::unique_ptr<GameObject>)> spawnCallback);

    void checkCollisions(std::vector<std::unique_ptr<GameObject>>& objects);

private:
    void setupCollisionHandlers();
    bool areColliding(const GameObject& obj1, const GameObject& obj2) const;

    Player& m_player;
    std::function<void(std::unique_ptr<GameObject>)> m_spawnCallback;
    MultiMethodCollisionHandler m_collisionHandler;
};