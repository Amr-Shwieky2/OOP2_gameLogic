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

#ifdef USE_VALIDATED_STATE_MACHINE
#include "StateMachine/PlayerStateMachine.h"
#include "StateMachine/StateMachineGuards.h"
#endif

#include <iostream>

// Function to initialize the state machine (defined in StateMachineDemo.cpp)
extern void InitializeStateMachine();

PlayerStateManager::PlayerStateManager(PlayerEntity& player)
    : m_player(player), m_currentState(nullptr) {
#ifdef USE_VALIDATED_STATE_MACHINE
    // Initialize the state machine for validation
    static bool initialized = false;
    if (!initialized) {
        InitializeStateMachine();
        initialized = true;
    }
#endif
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

#ifdef USE_VALIDATED_STATE_MACHINE
    // Validate transition using our state machine
    if (!PlayerStateMachine::ValidateTransition(m_currentState, newState)) {
        std::cerr << "[StateManager] ERROR: Invalid state transition from " 
                  << oldStateName << " to " << newStateName << std::endl;
        return; // Abort the invalid transition
    }
    
    // Check if guard conditions allow the transition
    if (!TransitionManager::CanTransition(m_currentState, newState, m_player)) {
        std::cerr << "[StateManager] Transition guard prevented state change from " 
                  << oldStateName << " to " << newStateName << std::endl;
        return; // Abort if guard rejects the transition
    }
    
    // Execute any transition actions
    TransitionManager::ExecuteAction(m_currentState, newState, m_player);
#endif

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
