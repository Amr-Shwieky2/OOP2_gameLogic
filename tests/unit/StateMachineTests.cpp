#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "StateMachine/StateMachine.h"
#include "StateMachine/PlayerStateMachine.h"
#include "StateMachine/ValidatedPlayerStateManager.h"
#include "PlayerEntity.h"

// Mock class for player state manager
class MockPlayerStateManager : public ValidatedPlayerStateManager {
public:
    MOCK_METHOD(void, handleTransition, (IState* oldState, IState* newState), (override));
    MOCK_METHOD(bool, validateTransition, (IState* oldState, IState* newState), (override));
};

// Test fixture for state machine tests
class StateMachineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize the state machine
        PlayerStateMachine::Initialize();
        
        // Create test player
        player = new PlayerEntity(1, "TestPlayer");
        
        // Create the state manager
        stateManager = new MockPlayerStateManager();
        stateManager->setPlayer(player);
    }
    
    void TearDown() override {
        delete stateManager;
        delete player;
    }
    
    PlayerEntity* player = nullptr;
    MockPlayerStateManager* stateManager = nullptr;
};

// Test basic state transitions
TEST_F(StateMachineTest, BasicTransitions) {
    // Start with normal state
    IState* normalState = NormalState::getInstance();
    stateManager->setState(normalState);
    EXPECT_EQ(normalState, stateManager->getCurrentState());
    
    // Transition to boosted state
    IState* boostedState = BoostedState::getInstance();
    
    // Expect the handleTransition method to be called
    EXPECT_CALL(*stateManager, handleTransition(normalState, boostedState))
        .Times(1);
    
    // Perform transition
    EXPECT_TRUE(stateManager->requestStateTransition(boostedState));
    EXPECT_EQ(boostedState, stateManager->getCurrentState());
}

// Test invalid state transitions
TEST_F(StateMachineTest, InvalidTransitions) {
    // Set up initial state
    IState* boostedState = BoostedState::getInstance();
    stateManager->setState(boostedState);
    
    // Try invalid transition (boosted to shielded)
    IState* shieldedState = ShieldedState::getInstance();
    
    // Expect validateTransition to be called and return false
    EXPECT_CALL(*stateManager, validateTransition(boostedState, shieldedState))
        .WillOnce(::testing::Return(false));
    
    // Transition should fail
    EXPECT_FALSE(stateManager->requestStateTransition(shieldedState));
    
    // State should not change
    EXPECT_EQ(boostedState, stateManager->getCurrentState());
}

// Test transition guards
TEST_F(StateMachineTest, TransitionGuards) {
    // Set up player for testing guards
    player->setHealth(50);
    
    // Create guard that requires health > 75
    auto healthGuard = [](PlayerEntity* player) -> bool {
        return player->getHealth() > 75;
    };
    
    // Register the guard for a transition
    PlayerStateMachine::RegisterTransitionGuard<NormalState, BoostedState>(healthGuard);
    
    // Set initial state
    IState* normalState = NormalState::getInstance();
    stateManager->setState(normalState);
    
    // Try to transition (should fail due to guard)
    IState* boostedState = BoostedState::getInstance();
    EXPECT_FALSE(PlayerStateMachine::CheckTransitionGuards(normalState, boostedState, player));
    
    // Update player health to pass guard
    player->setHealth(100);
    
    // Now the transition should be allowed
    EXPECT_TRUE(PlayerStateMachine::CheckTransitionGuards(normalState, boostedState, player));
}

// Test transition actions
TEST_F(StateMachineTest, TransitionActions) {
    // Set up player for testing actions
    player->setScore(0);
    
    // Create action that adds score
    auto scoreAction = [](PlayerEntity* player) {
        player->addScore(100);
    };
    
    // Register the action for a transition
    PlayerStateMachine::RegisterTransitionAction<NormalState, BoostedState>(scoreAction);
    
    // Set initial state
    IState* normalState = NormalState::getInstance();
    stateManager->setState(normalState);
    
    // Set up expectations
    EXPECT_CALL(*stateManager, handleTransition(normalState, ::testing::_))
        .Times(1)
        .WillOnce(::testing::Invoke([this, scoreAction](IState* oldState, IState* newState) {
            // Execute actions
            PlayerStateMachine::ExecuteTransitionActions(oldState, newState, player);
            // Update state
            stateManager->setState(newState);
        }));
    
    // Perform transition
    IState* boostedState = BoostedState::getInstance();
    stateManager->requestStateTransition(boostedState);
    
    // Check that the action was executed
    EXPECT_EQ(100, player->getScore());
}

// Test compile-time transition validation
TEST_F(StateMachineTest, CompileTimeValidation) {
    // These checks are performed at compile time
    constexpr bool normalToBoosted = PlayerStateMachine::IsValidTransition<NormalState, BoostedState>();
    constexpr bool boostedToNormal = PlayerStateMachine::IsValidTransition<BoostedState, NormalState>();
    constexpr bool normalToShielded = PlayerStateMachine::IsValidTransition<NormalState, ShieldedState>();
    
    // The validity should match our state machine configuration
    EXPECT_TRUE(normalToBoosted);
    EXPECT_TRUE(boostedToNormal);
    EXPECT_TRUE(normalToShielded);
    
    // This would fail at compile time if uncommented:
    // constexpr bool boostedToShielded = PlayerStateMachine::IsValidTransition<BoostedState, ShieldedState>();
}