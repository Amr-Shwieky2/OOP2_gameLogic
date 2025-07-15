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
#include <AudioManager.h>

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

void PlayerStateManager::applySpeedBoost(float ) {
    changeState(BoostedState::getInstance());
}

void PlayerStateManager::applyShield(float ) {
    changeState(ShieldedState::getInstance());
}

void PlayerStateManager::applyMagneticEffect(float ) {
    changeState(MagneticState::getInstance());
}

void PlayerStateManager::applyReverseEffect(float ) {
    changeState(ReversedState::getInstance());
}

void PlayerStateManager::applyHeadwindEffect(float ) {
    changeState(HeadwindState::getInstance());
}
