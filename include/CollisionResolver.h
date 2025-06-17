#pragma once

#include "GameObjectVisitor.h"
#include "Player.h"
#include <functional>

class CollisionResolver : public GameObjectVisitor {
public:
    CollisionResolver(Player& player, std::function<void(std::unique_ptr<GameObject>)> spawnCallback);

    // Gifts & Collectibles
    // In CollisionResolver.h
    void visit(Player& player) override;
    void visit(Coin& coin) override;
    void visit(LifeHeartGift& gift) override;
    void visit(SpeedGift& gift) override;
    void visit(ReverseMovementGift& gift) override;
    void visit(ProtectiveShieldGift& gift) override;
    void visit(HeadwindStormGift& gift) override;
    void visit(RareCoinGift& gift) override;

    // Interactive
    void visit(CloseBox& box) override;

private:
    Player& m_player;
    std::function<void(std::unique_ptr<GameObject>)> m_spawnCallback;
};
