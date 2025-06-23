#include "CollisionSystem.h"
#include "GameObject.h"
#include <iostream>
#include <cmath>

CollisionSystem::CollisionSystem(Player& player, GameState& gameState,
    std::function<void(std::unique_ptr<GameObject>)> spawnCallback)
    : m_player(player), m_gameState(gameState), m_spawnCallback(spawnCallback){}

void CollisionSystem::checkCollisions(std::vector<std::unique_ptr<GameObject>>& objects) {
    for (auto& obj : objects) {
        if (!areColliding(m_player, *obj)) {
            continue;
        }
        // Check for collectable items
        if (auto* collectable = dynamic_cast<ICollectable*>(obj.get())) {
            if (!collectable->isCollected()) {
                handlePlayerCollectableCollision(*collectable);
            }
        }

        // Check for movable boxes
        else if (auto* box = dynamic_cast<MovableBox*>(obj.get())) {
            handlePlayerBoxCollision(*box);
        }

        // Check for ground tiles
        else if (auto* ground = dynamic_cast<GroundTile*>(obj.get())) {
            handlePlayerGroundCollision(*ground);
        }
    }
}

void CollisionSystem::handlePlayerCollectableCollision(ICollectable& collectable) {
    // Let the collectable handle its own collection logic
    collectable.onCollect(m_gameState);

    // Apply any effects to the player
    PlayerEffect effect = collectable.getEffect();
    if (effect != PlayerEffect::None) {
        float duration = collectable.getEffectDuration();
        m_player.applyEffect(effect, duration);
    }
}

void CollisionSystem::handlePlayerBoxCollision(MovableBox& box) {
    // Calculate push direction and force
    sf::Vector2f pushDirection = calculatePushDirection(m_player, box);

    // Base push force
    float pushForce = 150.0f;

    // Increase force based on player velocity
    sf::Vector2f playerVelocity = m_player.getVelocity();
    float velocityMultiplier = std::abs(playerVelocity.x) / 100.0f;
    pushForce *= (1.0f + velocityMultiplier);

    // Apply speed boost effect to push force
    if (m_player.hasEffect(PlayerEffect::SpeedBoost)) {
        pushForce *= 1.5f;
    }

    // Apply the force to the box
    box.applyForce(pushDirection.x * pushForce, pushDirection.y * pushForce);
}

void CollisionSystem::handlePlayerGroundCollision(GroundTile& ground) {
    // Handle ground contact for jumping mechanics
    m_player.beginContact();
}

bool CollisionSystem::areColliding(const GameObject& obj1, const GameObject& obj2) const {
    sf::FloatRect bounds1 = obj1.getBounds();
    sf::FloatRect bounds2 = obj2.getBounds();
    return bounds1.intersects(bounds2);
}

sf::Vector2f CollisionSystem::calculatePushDirection(const GameObject& pusher, const GameObject& target) const {
    sf::FloatRect pusherBounds = pusher.getBounds();
    sf::FloatRect targetBounds = target.getBounds();

    float pusherCenterX = pusherBounds.left + pusherBounds.width / 2.0f;
    float targetCenterX = targetBounds.left + targetBounds.width / 2.0f;

    // Determine horizontal push direction
    float directionX = (pusherCenterX < targetCenterX) ? 1.0f : -1.0f;

    // No vertical push for ground-based objects
    float directionY = 0.0f;

    return sf::Vector2f(directionX, directionY);
}