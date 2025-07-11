#pragma once

#include <gtest/gtest.h>
#include "Entity.h"
#include "Component.h"
#include "Transform.h"
#include "mocks/MockComponent.h"

/**
 * Fixture for component-related tests
 * Provides a test entity and component setup
 */
class ComponentFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test entity
        entity = std::make_unique<Entity>(1);
        
        // Add a transform component
        transform = entity->addComponent<Transform>();
        transform->setPosition(100.0f, 100.0f);
    }
    
    void TearDown() override {
        // Clean up entity (which will also clean up components)
        entity.reset();
    }
    
    // Test entity and components
    std::unique_ptr<Entity> entity;
    Transform* transform = nullptr;
};