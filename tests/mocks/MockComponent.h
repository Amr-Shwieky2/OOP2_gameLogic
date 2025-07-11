#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Component.h"

/**
 * Mock component for testing component system
 */
class MockComponent : public Component {
public:
    MOCK_METHOD(void, initialize, (), (override));
    MOCK_METHOD(void, update, (float), (override));
    MOCK_METHOD(void, render, (sf::RenderTarget&), (override));
    MOCK_METHOD(const char*, getType, (), (const, override));

    MockComponent() {
        // Set default behaviors
        ON_CALL(*this, getType())
            .WillByDefault(testing::Return("MockComponent"));
    }

    // Make component creation public for testing
    using Component::create;
};