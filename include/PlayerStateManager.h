#pragma once
#include "PlayerState.h"
#include <memory>

class PlayerEntity;

/**
 * PlayerStateManager - Single Responsibility: Manage player state transitions
 */
class PlayerStateManager {
public:
    PlayerStateManager(PlayerEntity& player);

    void update(float dt);
    void changeState(PlayerState* newState);
    PlayerState* getCurrentState() const { return m_currentState; }

    // State effect applications
    void applySpeedBoost(float duration);
    void applyShield(float duration);
    void applyMagneticEffect(float duration);
    void applyReverseEffect(float duration);
    void applyHeadwindEffect(float duration);

private:
    PlayerEntity& m_player;
    PlayerState* m_currentState = nullptr;
};
