#include "CollisionSystem.h"
#include <iostream>

CollisionSystem::CollisionSystem(Player& player, std::function<void(std::unique_ptr<GameObject>)> spawnCallback)
    : m_player(player), m_spawnCallback(spawnCallback) {
    setupCollisionHandlers();
    std::cout << "CollisionSystem created with multimethods!" << std::endl;
}

void CollisionSystem::setupCollisionHandlers() {
    // Player + Coin - جمع العملات
    m_collisionHandler.registerHandler<Player, Coin>(
        [](Player& player, Coin& coin) {
            if (!coin.isCollected()) {
                coin.collect();
                player.increaseScore(10);
                std::cout << "Coin collected! Score: " << player.getScore() << std::endl;
            }
        }
    );

    // Player + LifeHeartGift - زيادة الأرواح
    m_collisionHandler.registerHandler<Player, LifeHeartGift>(
        [](Player& player, LifeHeartGift& gift) {
            if (!gift.isCollected()) {
                gift.onCollect(player);
                player.addLife();
                std::cout << "Life gained! Lives: " << player.getLives() << std::endl;
            }
        }
    );

    // Player + SpeedGift - زيادة السرعة
    m_collisionHandler.registerHandler<Player, SpeedGift>(
        [](Player& player, SpeedGift& gift) {
            if (!gift.isCollected()) {
                gift.onCollect(player);
                player.applyEffect(PlayerEffect::SpeedBoost, 6.0f);
                std::cout << "Speed boost activated!" << std::endl;
            }
        }
    );

    // Player + ReverseMovementGift - عكس الحركة
    m_collisionHandler.registerHandler<Player, ReverseMovementGift>(
        [](Player& player, ReverseMovementGift& gift) {
            if (!gift.isCollected()) {
                gift.onCollect(player);
                player.applyEffect(PlayerEffect::ReverseControl, 5.0f);
                std::cout << "Controls reversed!" << std::endl;
            }
        }
    );

    // Player + ProtectiveShieldGift - الحماية
    m_collisionHandler.registerHandler<Player, ProtectiveShieldGift>(
        [](Player& player, ProtectiveShieldGift& gift) {
            if (!gift.isCollected()) {
                gift.onCollect(player);
                player.applyEffect(PlayerEffect::Shield, 8.0f);
                std::cout << "Shield activated!" << std::endl;
            }
        }
    );

    // Player + HeadwindStormGift - مقاومة الرياح
    m_collisionHandler.registerHandler<Player, HeadwindStormGift>(
        [](Player& player, HeadwindStormGift& gift) {
            if (!gift.isCollected()) {
                gift.onCollect(player);
                player.applyEffect(PlayerEffect::Headwind, 5.0f);
                std::cout << "Headwind resistance!" << std::endl;
            }
        }
    );

    // Player + RareCoinGift - العملة النادرة
    m_collisionHandler.registerHandler<Player, RareCoinGift>(
        [](Player& player, RareCoinGift& gift) {
            if (!gift.isCollected()) {
                gift.onCollect(player);
                player.increaseScore(50); // عملة نادرة = 50 نقطة
                std::cout << "Rare coin collected! +50 points!" << std::endl;
            }
        }
    );

    // Player + CloseBox - فتح الصندوق
    m_collisionHandler.registerHandler<Player, CloseBox>(
        [this](Player& player, CloseBox& box) {
            if (!box.isOpened()) {
                box.open();
                std::cout << "📦 Box opened!" << std::endl;

                // إنشاء عملة نادرة من الصندوق
                float x = box.getBounds().left;
                float y = box.getBounds().top;
                m_spawnCallback(std::make_unique<RareCoinGift>(x, y, player.getTextureManager()));
            }
        }
    );
}

void CollisionSystem::checkCollisions(std::vector<std::unique_ptr<GameObject>>& objects) {
    for (auto& obj : objects) {
        if (areColliding(m_player, *obj)) {
            // استخدام multimethods لمعالجة التصادم
            bool handled = m_collisionHandler.handleCollision(m_player, *obj);

            if (!handled) {
                // لا يوجد handler لهذا النوع من التصادم
                // std::cout << "No collision handler for this object type" << std::endl;
            }
        }
    }
}

bool CollisionSystem::areColliding(const GameObject& obj1, const GameObject& obj2) const {
    sf::FloatRect bounds1 = obj1.getBounds();
    sf::FloatRect bounds2 = obj2.getBounds();
    return bounds1.intersects(bounds2);
}