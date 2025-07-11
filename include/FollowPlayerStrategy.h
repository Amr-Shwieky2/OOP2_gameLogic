// FollowPlayerStrategy.h
#pragma once
#include "AIStrategy.h"
#include <SFML/System/Vector2.hpp>

/**
 * FollowPlayerStrategy - Enemy follows the player
 * Updated to work with specialized AIComponent
 */
class FollowPlayerStrategy : public AIStrategy {
public:
    FollowPlayerStrategy(float speed = 100.0f, float detectionRange = 300.0f);

    // AIStrategy interface
    void update(float dt) override;
    void onTargetDetected(PlayerEntity* player) override;
    void onTargetLost(const sf::Vector2f& lastKnownPosition) override;
    const char* getName() const override { return "FollowPlayer"; }

private:
    float m_speed;
    float m_detectionRange;
    PlayerEntity* m_targetPlayer = nullptr;
    sf::Vector2f m_lastKnownPosition;
    bool m_isChasing = false;
};