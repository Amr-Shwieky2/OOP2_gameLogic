// PatrolStrategy.h
#pragma once
#include "AIStrategy.h"

/**
 * PatrolStrategy - Enemy moves back and forth
 */
class PatrolStrategy : public AIStrategy {
public:
    PatrolStrategy(float patrolDistance = 200.0f, float speed = 50.0f);

    void update(Entity& entity, float dt, PlayerEntity* player) override;
    const char* getName() const override { return "Patrol"; }

private:
    float m_patrolDistance;
    float m_speed;
    float m_startX = 0.0f;
    int m_direction = 1;
    bool m_initialized = false;
};