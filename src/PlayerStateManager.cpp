#include "PlayerStateManager.h"
#include "PlayerEntity.h"
#include "PlayerState.h"
#include "NormalState.h"
#include "ShieldedState.h"
#include "BoostedState.h"
#include "MagneticState.h"
#include "ReversedState.h"
#include "HeadwindState.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include <iostream>

PlayerStateManager::PlayerStateManager(PlayerEntity& player)
    : m_player(player), m_currentState(nullptr) {
}

void PlayerStateManager::update(float dt) {
    if (m_currentState) {
        m_currentState->update(m_player, dt);
    }
}

void PlayerStateManager::changeState(PlayerState* newState) {
    if (m_currentState == newState) return;

    std::string oldStateName = m_currentState ? m_currentState->getName() : "None";
    std::string newStateName = newState ? newState->getName() : "None";

    std::cout << "[StateManager] State change: " << oldStateName
        << " -> " << newStateName << std::endl;

    // Exit current state
    if (m_currentState) {
        m_currentState->exit(m_player);
    }

    // Change to new state
    m_currentState = newState;

    // Enter new state
    if (m_currentState) {
        m_currentState->enter(m_player);
    }

    // Publish state change event
    EventSystem::getInstance().publish(
        PlayerStateChangedEvent(oldStateName, newStateName)
    );
}

void PlayerStateManager::applySpeedBoost(float duration) {
    std::cout << "[StateManager] Applying speed boost for " << duration << " seconds" << std::endl;
    changeState(BoostedState::getInstance());
}

void PlayerStateManager::applyShield(float duration) {
    std::cout << "[StateManager] Applying shield for " << duration << " seconds" << std::endl;
    changeState(ShieldedState::getInstance());
}

void PlayerStateManager::applyMagneticEffect(float duration) {
    std::cout << "[StateManager] Applying magnetic effect for " << duration << " seconds" << std::endl;
    changeState(MagneticState::getInstance());
}

void PlayerStateManager::applyReverseEffect(float duration) {
    std::cout << "[StateManager] Applying reverse effect for " << duration << " seconds" << std::endl;
    changeState(ReversedState::getInstance());
}

void PlayerStateManager::applyHeadwindEffect(float duration) {
    std::cout << "[StateManager] Applying headwind effect for " << duration << " seconds" << std::endl;
    changeState(HeadwindState::getInstance());
}
