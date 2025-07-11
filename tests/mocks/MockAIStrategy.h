#pragma once
#include <gmock/gmock.h>
#include "AIStrategy.h"

/**
 * @brief Mock class for testing AI strategies
 */
class MockAIStrategy : public AIStrategy {
public:
    MOCK_METHOD(void, update, (float dt), (override));
    MOCK_METHOD(void, onTargetDetected, (PlayerEntity* player), (override));
    MOCK_METHOD(void, onTargetLost, (const sf::Vector2f& lastKnownPosition), (override));
    MOCK_METHOD(const char*, getName, (), (const, override));
    MOCK_METHOD(bool, canSeePlayer, (const sf::Vector2f& entityPos, const sf::Vector2f& playerPos), (const, override));
    
    MockAIStrategy() {
        // Set default behaviors
        ON_CALL(*this, getName())
            .WillByDefault(::testing::Return("MockStrategy"));
    }

    // Helper method to set up default behavior
    void setupDefaultBehavior() {
        using ::testing::_;
        using ::testing::Return;
        
        ON_CALL(*this, getName())
            .WillByDefault(Return("MockAIStrategy"));
            
        ON_CALL(*this, canSeePlayer(_, _))
            .WillByDefault(::testing::Invoke(this, &MockAIStrategy::AIStrategy::canSeePlayer));
    }
};