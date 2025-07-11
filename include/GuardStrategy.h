// GuardStrategy.h
#pragma once
#include "AIStrategy.h"

/**
 * GuardStrategy - Enemy guards a position and attacks nearby players
 * Updated to work with specialized AIComponent
 */
class GuardStrategy : public AIStrategy {
public:
    GuardStrategy(float guardRadius = 150.0f, float attackRange = 100.0f);

    // AIStrategy interface
    void update(float dt) override;
    void onTargetDetected(PlayerEntity* player) override;
    const char* getName() const override { return "Guard"; }

private:
    float m_guardRadius;
    float m_attackRange;
    sf::Vector2f m_guardPosition;
    bool m_initialized = false;
    float m_attackCooldown = 0.0f;
    PlayerEntity* m_targetPlayer = nullptr;
};