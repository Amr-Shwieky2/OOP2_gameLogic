#include "StateMachine/StateMachine.h"
#include "StateMachine/PlayerStateMachine.h"
#include "StateMachine/StateMachineGuards.h"
#include "StateMachine/StateMachineDSL.h"
#include "StateMachine/StateMachineDiagram.h"
#include "StateMachine/StateMachineTests.h"
#include "StateMachine/ValidatedPlayerStateManager.h"
#include "PlayerEntity.h"
#include "NormalState.h"
#include "BoostedState.h"
#include "ShieldedState.h"
#include <iostream>

// Forward declarations of functions defined elsewhere
extern void InitializeStateMachine();

/**
 * @brief Demonstrate the state machine with a ValidatedPlayerStateManager
 */
void demonstrateStateMachine() {
    std::cout << "State Machine Demonstration\n";
    std::cout << "==========================\n";
    
    // Initialize state machine
    InitializeStateMachine();
    
    // Create a simple player entity for testing
    // This is a simplified version, in a real game we'd use the actual PlayerEntity
    class MockPlayerEntity : public PlayerEntity {
    public:
        MockPlayerEntity() : PlayerEntity(1, *(new b2World(b2Vec2(0, 0))), 0, 0, *(new TextureManager())) {}
        ~MockPlayerEntity() {}
    };
    
    try {
        // Create a mock player and state manager
        MockPlayerEntity player;
        ValidatedPlayerStateManager stateManager(player);
        
        // Set initial state
        stateManager.changeState(NormalState::getInstance());
        
        // Demonstrate valid transitions
        std::cout << "\nPerforming valid transitions:\n";
        
        std::cout << "Normal -> Boosted\n";
        stateManager.changeState(BoostedState::getInstance());
        
        std::cout << "Boosted -> Normal\n";
        stateManager.changeState(NormalState::getInstance());
        
        std::cout << "Normal -> Shielded\n";
        stateManager.changeState(ShieldedState::getInstance());
        
        std::cout << "Shielded -> Normal\n";
        stateManager.changeState(NormalState::getInstance());
        
        // Try an invalid transition
        std::cout << "\nAttempting invalid transition:\n";
        std::cout << "Attempting Boosted -> Shielded (should fail)\n";
        
        stateManager.changeState(BoostedState::getInstance());
        try {
            // This should throw an exception
            stateManager.changeStateTyped<BoostedState, ShieldedState>(ShieldedState::getInstance());
        } catch (const InvalidStateTransitionException& e) {
            std::cout << "Caught expected exception: " << e.what() << std::endl;
        }
        
        std::cout << "\nState machine demonstration completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

/**
 * @brief Main entry point for state machine testing
 */
int runStateMachineTests() {
    try {
        std::cout << "Running state machine tests...\n";
        
        // Run the basic tests first
        int failures = StateMachineTests::RunTests();
        
        // Then demonstrate the state machine in action
        demonstrateStateMachine();
        
        // Generate a visualization of the state machine
        std::cout << "\nGenerating state machine diagram...\n";
        GenerateStateMachineDiagram<PlayerStateMachine>("player_state_machine.dot");
        std::cout << "Diagram written to player_state_machine.dot\n";
        std::cout << "Use 'dot -Tpng player_state_machine.dot -o player_state_machine.png' to convert to PNG\n";
        
        return failures;
    } catch (const std::exception& e) {
        std::cerr << "ERROR during state machine tests: " << e.what() << std::endl;
        return 1;
    }
}