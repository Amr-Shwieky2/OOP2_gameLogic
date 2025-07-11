#pragma once
#include "StateMachine/StateMachine.h"
#include "PlayerState.h"
#include "NormalState.h"
#include "BoostedState.h"
#include "ShieldedState.h"
#include "MagneticState.h"
#include "ReversedState.h"
#include "HeadwindState.h"

/**
 * @brief PlayerStateMachine - Defines valid state transitions for PlayerState
 * 
 * This class uses compile-time template metaprogramming to define and validate
 * state transitions for the player character.
 */
class PlayerStateMachine {
public:
    // Define all possible state transitions using a domain-specific language
    using Transitions = std::tuple<
        // From Normal state
        StateMachine<PlayerState, void>::Transition<NormalState, BoostedState>,
        StateMachine<PlayerState, void>::Transition<NormalState, ShieldedState>,
        StateMachine<PlayerState, void>::Transition<NormalState, MagneticState>,
        StateMachine<PlayerState, void>::Transition<NormalState, ReversedState>,
        StateMachine<PlayerState, void>::Transition<NormalState, HeadwindState>,
        
        // From Boosted state
        StateMachine<PlayerState, void>::Transition<BoostedState, NormalState>,
        
        // From Shielded state
        StateMachine<PlayerState, void>::Transition<ShieldedState, NormalState>,
        
        // From Magnetic state
        StateMachine<PlayerState, void>::Transition<MagneticState, NormalState>,
        
        // From Reversed state
        StateMachine<PlayerState, void>::Transition<ReversedState, NormalState>,
        
        // From Headwind state
        StateMachine<PlayerState, void>::Transition<HeadwindState, NormalState>
        
        // Additional transitions can be added here
    >;
    
    // Define the state machine type
    using Machine = StateMachine<PlayerState, Transitions>;
    
    /**
     * @brief Initialize the state machine
     * 
     * This function registers all valid transitions in the runtime registry.
     */
    static void Initialize() {
        // Register all transitions defined in the Transitions tuple
        RegisterTransition<NormalState, BoostedState>();
        RegisterTransition<NormalState, ShieldedState>();
        RegisterTransition<NormalState, MagneticState>();
        RegisterTransition<NormalState, ReversedState>();
        RegisterTransition<NormalState, HeadwindState>();
        
        RegisterTransition<BoostedState, NormalState>();
        RegisterTransition<ShieldedState, NormalState>();
        RegisterTransition<MagneticState, NormalState>();
        RegisterTransition<ReversedState, NormalState>();
        RegisterTransition<HeadwindState, NormalState>();
        
        std::cout << "[PlayerStateMachine] State transitions initialized" << std::endl;
    }
    
    /**
     * @brief Validate a transition at compile-time
     * 
     * @tparam From Source state type
     * @tparam To Destination state type
     * @return constexpr bool True if transition is valid
     */
    template<typename From, typename To>
    static constexpr bool IsValidTransition() {
        return Machine::template IsValidTransition<From, To>();
    }
    
    /**
     * @brief Validate a transition at runtime
     * 
     * @param from Source state pointer
     * @param to Destination state pointer
     * @return true if transition is valid, false otherwise
     */
    static bool ValidateTransition(const PlayerState* from, const PlayerState* to) {
        return Machine::ValidateTransition(from, to);
    }
};

// Static assertions to validate state transitions at compile time
static_assert(PlayerStateMachine::IsValidTransition<NormalState, BoostedState>(), 
              "Transition from NormalState to BoostedState should be valid");

static_assert(PlayerStateMachine::IsValidTransition<BoostedState, NormalState>(), 
              "Transition from BoostedState to NormalState should be valid");

// This would fail compilation if uncommented, as it's an invalid transition
// static_assert(PlayerStateMachine::IsValidTransition<BoostedState, ShieldedState>(), 
//              "Transition from BoostedState to ShieldedState should be invalid");