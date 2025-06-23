#include "MultiMethodCollisionSystem.h"
#include <cmath>

MultiMethodCollisionSystem::MultiMethodCollisionSystem(Player& player, GameState& gameState)
    : m_player(player), m_gameState(gameState) {
    registerAllHandlers();
}

bool MultiMethodCollisionSystem::handleCollision(GameObject& obj1, GameObject& obj2) {
    if (!areColliding(obj1, obj2)) {
        return false;
    }

    TypePair key = makeTypePair(obj1, obj2);

    auto it = m_handlers.find(key);
    if (it != m_handlers.end()) {
        it->second(obj1, obj2);
        return true;
    }

    TypePair reverseKey = { key.second, key.first };
    auto reverseIt = m_handlers.find(reverseKey);
    if (reverseIt != m_handlers.end()) {
        reverseIt->second(obj2, obj1);
        return true;
    }

    return false; // لا يوجد handler مُسجل
}

void MultiMethodCollisionSystem::checkAllCollisions(std::vector<std::unique_ptr<GameObject>>& objects) {
    // Player vs Everything
    for (auto& obj : objects) {
        handleCollision(m_player, *obj);
    }
}

void MultiMethodCollisionSystem::registerAllHandlers() {
    // Player vs Collectables
    registerHandler<Player, Coin>([this](Player& player, Coin& coin) {
        handlePlayerCoinCollision(player, coin);
        });

    registerHandler<Player, Flag>([this](Player& player, Flag& flag) {
        handlePlayerFlagCollision(player, flag);
        });

    // Player vs Interactive Objects
    registerHandler<Player, MovableBox>([this](Player& player, MovableBox& box) {
        handlePlayerBoxCollision(player, box);
        });

    registerHandler<Player, GroundTile>([this](Player& player, GroundTile& ground) {
        handlePlayerGroundCollision(player, ground);
        });
}

// ===== Collision Handlers =====

void MultiMethodCollisionSystem::handlePlayerCoinCollision(Player& player, Coin& coin) {
    if (!coin.isCollected()) {

        coin.onCollect(m_gameState);

        // تطبيق التأثيرات
        PlayerEffect effect = coin.getEffect();
        if (effect != PlayerEffect::None) {
            player.applyEffect(effect, coin.getEffectDuration());
        }
    }
}

void MultiMethodCollisionSystem::handlePlayerFlagCollision(Player& player, Flag& flag) {
    m_gameState.setLevelComplete(true);
}

void MultiMethodCollisionSystem::handlePlayerBoxCollision(Player& player, MovableBox& box) {

    // حساب اتجاه الدفع
    sf::Vector2f pushDirection = calculatePushDirection(player, box);

    // قوة الدفع الأساسية
    float pushForce = 150.0f;

    // زيادة القوة حسب سرعة اللاعب
    sf::Vector2f playerVelocity = player.getVelocity();
    float velocityMultiplier = std::abs(playerVelocity.x) / 100.0f;
    pushForce *= (1.0f + velocityMultiplier);

    // تطبيق تأثير Speed Boost
    if (player.hasEffect(PlayerEffect::SpeedBoost)) {
        pushForce *= 1.5f;
    }

    // تطبيق القوة
    box.applyForce(pushDirection.x * pushForce, pushDirection.y * pushForce);
}

void MultiMethodCollisionSystem::handlePlayerGroundCollision(Player& player, GroundTile& ground) {
    // معالجة اتصال الأرض (للقفز)
    player.beginContact();
}

// ===== Helper Methods =====

MultiMethodCollisionSystem::TypePair MultiMethodCollisionSystem::makeTypePair(GameObject& obj1, GameObject& obj2) {
    return { std::type_index(typeid(obj1)), std::type_index(typeid(obj2)) };
}

bool MultiMethodCollisionSystem::areColliding(const GameObject& obj1, const GameObject& obj2) const {
    sf::FloatRect bounds1 = obj1.getBounds();
    sf::FloatRect bounds2 = obj2.getBounds();
    return bounds1.intersects(bounds2);
}

sf::Vector2f MultiMethodCollisionSystem::calculatePushDirection(const GameObject& pusher, const GameObject& target) const {
    sf::FloatRect pusherBounds = pusher.getBounds();
    sf::FloatRect targetBounds = target.getBounds();

    float pusherCenterX = pusherBounds.left + pusherBounds.width / 2.0f;
    float targetCenterX = targetBounds.left + targetBounds.width / 2.0f;

    float directionX = (pusherCenterX < targetCenterX) ? 1.0f : -1.0f;
    float directionY = 0.0f; // حركة أفقية فقط

    return sf::Vector2f(directionX, directionY);
}