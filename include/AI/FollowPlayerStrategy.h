// FollowPlayerStrategy.h
#pragma once
#include "AIStrategy.h"

/**
 * FollowPlayerStrategy - Enemy follows the player
 */
class FollowPlayerStrategy : public AIStrategy {
public:
    FollowPlayerStrategy(float speed = 100.0f, float detectionRange = 300.0f);

    void update(Entity& entity, float dt, PlayerEntity* player) override;
    const char* getName() const override { return "FollowPlayer"; }

private:
    float m_speed;
    float m_detectionRange;
};