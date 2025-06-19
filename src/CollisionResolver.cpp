// CollisionResolver.cpp
#include "CollisionResolver.h"

#include "Coin.h"
#include "LifeHeartGift.h"
#include "SpeedGift.h"
#include "ReverseMovementGift.h"
#include "ProtectiveShieldGift.h"
#include "HeadwindStormGift.h"
#include "RareCoinGift.h"
#include "CloseBox.h"

CollisionResolver::CollisionResolver(Player& player, std::function<void(std::unique_ptr<GameObject>)> spawnCallback)
    : m_player(player), m_spawnCallback(std::move(spawnCallback)) {
}

// Default no-op for player (no self-collision handling)
void CollisionResolver::visit(Player&) {}

// Coin pickup logic
void CollisionResolver::visit(Coin& coin) {
    if (!coin.isCollected()) {
        coin.collect();
        m_player.increaseScore(1);
    }
}

// Heart -> extra life
void CollisionResolver::visit(LifeHeartGift& gift) {
    if (!gift.isCollected()) {
        gift.onCollect(m_player);
        m_player.addLife();
    }
}

// Speed boost
void CollisionResolver::visit(SpeedGift& gift) {
    if (!gift.isCollected()) {
        gift.onCollect(m_player);
        m_player.applyEffect(PlayerEffect::SpeedBoost, 6.0f);
    }
}

// Reverse movement
void CollisionResolver::visit(ReverseMovementGift& gift) {
    if (!gift.isCollected()) {
        gift.onCollect(m_player);
        m_player.applyEffect(PlayerEffect::ReverseControl, 5.0f);
    }
}

// Protective shield
void CollisionResolver::visit(ProtectiveShieldGift& gift) {
    if (!gift.isCollected()) {
        gift.onCollect(m_player);
        m_player.applyEffect(PlayerEffect::Shield, 8.0f);
    }
}

// Headwind resistance
void CollisionResolver::visit(HeadwindStormGift& gift) {
    if (!gift.isCollected()) {
        gift.onCollect(m_player);
        m_player.applyEffect(PlayerEffect::Headwind, 5.0f);
    }
}

// Rare coin bonus
void CollisionResolver::visit(RareCoinGift& gift) {
    if (!gift.isCollected()) {
        gift.onCollect(m_player);
        m_player.increaseScore(10); // Rare coin worth more points
    }
}

// CloseBox contains surprises
void CollisionResolver::visit(CloseBox& box) {
    if (!box.isOpened()) {
        box.open();

        // Always spawns RareCoinGift for simplicity (can be randomized)
        float x = box.getBounds().left;
        float y = box.getBounds().top;

        m_spawnCallback(std::make_unique<RareCoinGift>(x, y, m_player.getTextureManager()));
    }
}
