#pragma once
#include "PlayerStateManager.h"
#include "StateMachine/PlayerStateMachine.h"
#include "StateMachine/StateMachineGuards.h"
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <iostream>

/**
 * @brief Exception thrown when an invalid state transition is attempted
 */
class InvalidStateTransitionException : public std::runtime_error {
public:
    InvalidStateTransitionException(const std::string& from, const std::string& to)
        : std::runtime_error("Invalid state transition from '" + from + "' to '" + to + "'") {}
};

/**
 * @brief Enhanced PlayerStateManager with compile-time state validation
 * 
 * This class extends the original PlayerStateManager to add compile-time
 * and runtime validation of state transitions.
 */
class ValidatedPlayerStateManager : public PlayerStateManager {
public:
    /**
     * @brief Construct a new validated state manager
     * @param player Player entity
     */
    explicit ValidatedPlayerStateManager(PlayerEntity& player)
        : PlayerStateManager(player), m_player(player) {
        // Initialize the state machine
        PlayerStateMachine::Initialize();
    }
    
    /**
     * @brief Change the player state with validation
     * @param newState New state to transition to
     * @throws InvalidStateTransitionException if the transition is not allowed
     */
    void changeState(PlayerState* newState) override {
        PlayerState* currentState = getCurrentState();
        
        // First, validate the transition
        if (!PlayerStateMachine::ValidateTransition(currentState, newState)) {
            std::string fromName = currentState ? currentState->getName() : "None";
            std::string toName = newState ? newState->getName() : "None";
            
            std::cerr << "[ValidatedStateManager] Invalid state transition: " 
                      << fromName << " -> " << toName << std::endl;
            
            throw InvalidStateTransitionException(fromName, toName);
        }
        
        // Then check if guard conditions allow the transition
        if (!TransitionManager::CanTransition(currentState, newState, m_player)) {
            std::string fromName = currentState ? currentState->getName() : "None";
            std::string toName = newState ? newState->getName() : "None";
            
            std::cerr << "[ValidatedStateManager] Transition guard rejected: " 
                      << fromName << " -> " << toName << std::endl;
            
            return; // Don't perform the transition
        }
        
        // Execute transition action before changing state
        TransitionManager::ExecuteAction(currentState, newState, m_player);
        
        // Proceed with the state change
        PlayerStateManager::changeState(newState);
    }
    
    /**
     * @brief Template method for type-safe state transitions
     * @tparam From Source state type
     * @tparam To Destination state type
     * @param newState New state instance
     * 
     * This method validates the transition at compile time using C++ templates
     */
    template<typename From, typename To>
    void changeStateTyped(To* newState) {
        // Verify current state type
        PlayerState* currentState = getCurrentState();
        if (!currentState || !dynamic_cast<From*>(currentState)) {
            std::cerr << "[ValidatedStateManager] Current state is not of expected type" << std::endl;
            return;
        }
        
        // Static validation at compile time (will not compile if invalid)
        static_assert(PlayerStateMachine::IsValidTransition<From, To>(),
                    "Invalid state transition detected at compile time");
        
        // Dynamic validation and state change
        changeState(newState);
    }
    
private:
    PlayerEntity& m_player;
};