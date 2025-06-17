#include "CollisionResolver.h"
#include "Coin.h"
#include "LifeHeartGift.h"
#include "SpeedGift.h"
#include "ReverseMovementGift.h"
#include "ProtectiveShieldGift.h"
#include "HeadwindStormGift.h"
#include "CloseBox.h"
#include "RareCoinGift.h"
#include "ResourceManager.h"

CollisionResolver::CollisionResolver(Player& player, std::function<void(std::unique_ptr<GameObject>)> spawnCallback)
    : m_player(player), m_spawnCallback(std::move(spawnCallback)) {
}

// ----------- Collectibles -----------

void CollisionResolver::visit(Player& player)
{
}

void CollisionResolver::visit(Coin& coin) {
    if (!coin.isCollected()) {
        m_player.addScore(10);
        coin.collect();
    }
}

void CollisionResolver::visit(LifeHeartGift& gift) {
    if (!gift.isCollected()) {
        m_player.addLife();
        gift.collect();
    }
}

void CollisionResolver::visit(SpeedGift& gift) {
    if (!gift.isCollected()) {
        m_player.applyEffect(PlayerEffect::SpeedBoost, 5.0f);
        gift.collect();
    }
}

void CollisionResolver::visit(ReverseMovementGift& gift) {
    if (!gift.isCollected()) {
        m_player.applyEffect(PlayerEffect::ReverseControl, 5.0f);
        gift.collect();
    }
}

void CollisionResolver::visit(ProtectiveShieldGift& gift) {
    if (!gift.isCollected()) {
        m_player.applyEffect(PlayerEffect::Shield, 7.0f);
        gift.collect();
    }
}

void CollisionResolver::visit(HeadwindStormGift& gift) {
    if (!gift.isCollected()) {
        m_player.applyEffect(PlayerEffect::Headwind, 5.0f);
        gift.collect();
    }
}

void CollisionResolver::visit(RareCoinGift& gift) {
    if (!gift.isCollected()) {
        m_player.addScore(20);
        gift.collect();

        int roll = rand() % 2;
        if (roll == 0)
            m_player.applyEffect(PlayerEffect::Magnetic, 6.0f);
        else
            m_player.applyEffect(PlayerEffect::Transparent, 6.0f);
    }
}

// ----------- Interactive Objects -----------

void CollisionResolver::visit(CloseBox& box) {
    if (!box.isOpened()) {
        box.open();

        sf::Vector2f pos = box.getBounds().getPosition();
        auto& textures = m_player.getTextureManager();  // Assumes this getter exists
        m_spawnCallback(std::make_unique<RareCoinGift>(pos.x, pos.y - 50.f, textures));
    }
}
