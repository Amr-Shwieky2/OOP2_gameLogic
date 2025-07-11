#include "StateMachine/StateMachine.h"
#include "StateMachine/PlayerStateMachine.h"
#include "StateMachine/StateMachineGuards.h"
#include "StateMachine/StateMachineDSL.h"
#include "StateMachine/StateMachineDiagram.h"
#include "StateMachine/StateMachineTests.h"
#include "PlayerEntity.h"
#include "HealthComponent.h"
#include <iostream>

/**
 * @brief Example of defining a state machine using the DSL
 */
void DefinePlayerStateMachine() {
    // Define the player state machine using the DSL
    STATE_MACHINE(playerStateMachine);
    
    // Define transitions from Normal state
    playerStateMachine.TRANSITION(NormalState, BoostedState, "Speed Boost")
        .WHEN("CanBoost", [](const PlayerEntity& player) {
            // Check if player is eligible for speed boost
            return true;
        })
        .DO("ApplyBoostEffect", [](PlayerEntity& player) {
            std::cout << "[Action] Applying boost effect" << std::endl;
        });
        
    playerStateMachine.TRANSITION(NormalState, ShieldedState, "Shield Activation")
        .WHEN("CanShield", [](const PlayerEntity& player) {
            // Check if player can be shielded
            auto* health = player.getComponent<HealthComponent>();
            return health && health->getHealth() > 0;
        })
        .DO("ApplyShieldEffect", [](PlayerEntity& player) {
            std::cout << "[Action] Applying shield effect" << std::endl;
        });
        
    playerStateMachine.TRANSITION(NormalState, MagneticState, "Magnet Activation")
        .WHEN("CanActivateMagnet", [](const PlayerEntity&) { return true; })
        .DO("ApplyMagneticEffect", [](PlayerEntity&) { 
            std::cout << "[Action] Applying magnetic effect" << std::endl;
        });
        
    playerStateMachine.TRANSITION(NormalState, ReversedState, "Control Reversal")
        .WHEN("CanReverseControls", [](const PlayerEntity&) { return true; })
        .DO("ApplyReverseEffect", [](PlayerEntity&) { 
            std::cout << "[Action] Applying control reversal effect" << std::endl;
        });
        
    playerStateMachine.TRANSITION(NormalState, HeadwindState, "Headwind Effect")
        .WHEN("CanApplyHeadwind", [](const PlayerEntity&) { return true; })
        .DO("ApplyHeadwindEffect", [](PlayerEntity&) { 
            std::cout << "[Action] Applying headwind effect" << std::endl;
        });
        
    // Define transitions back to Normal state
    playerStateMachine.TRANSITION(BoostedState, NormalState, "Boost Expired")
        .DO("RemoveBoostEffect", [](PlayerEntity&) { 
            std::cout << "[Action] Removing boost effect" << std::endl;
        });
        
    playerStateMachine.TRANSITION(ShieldedState, NormalState, "Shield Expired")
        .DO("RemoveShieldEffect", [](PlayerEntity&) { 
            std::cout << "[Action] Removing shield effect" << std::endl;
        });
        
    playerStateMachine.TRANSITION(MagneticState, NormalState, "Magnet Expired")
        .DO("RemoveMagneticEffect", [](PlayerEntity&) { 
            std::cout << "[Action] Removing magnetic effect" << std::endl;
        });
        
    playerStateMachine.TRANSITION(ReversedState, NormalState, "Reversal Expired")
        .DO("RemoveReverseEffect", [](PlayerEntity&) { 
            std::cout << "[Action] Removing control reversal effect" << std::endl;
        });
        
    playerStateMachine.TRANSITION(HeadwindState, NormalState, "Headwind Expired")
        .DO("RemoveHeadwindEffect", [](PlayerEntity&) { 
            std::cout << "[Action] Removing headwind effect" << std::endl;
        });
    
    // Build and register all transitions
    playerStateMachine.Build();
    
    // Generate diagram
    GenerateStateMachineDiagram<PlayerStateMachine>("player_state_machine.dot");
    
    std::cout << "Player state machine defined and diagram generated.\n";
    std::cout << "Use 'dot -Tpng player_state_machine.dot -o player_state_machine.png' to create PNG image\n";
}

/**
 * @brief Initialize the state machine and run tests
 */
void InitializeStateMachine() {
    std::cout << "Initializing player state machine with validation..." << std::endl;
    
    // Define the state machine using DSL
    DefinePlayerStateMachine();
    
    // Run tests
    int failures = StateMachineTests::RunTests();
    
    if (failures == 0) {
        std::cout << "All state machine tests passed!" << std::endl;
    } else {
        std::cerr << failures << " state machine tests failed!" << std::endl;
    }
}