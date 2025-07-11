#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Entity.h"
#include "Component.h"
#include "mocks/MockComponent.h"

class EntityTest : public ::testing::Test {
protected:
    void SetUp() override {
        entity = new Entity();
    }
    
    void TearDown() override {
        delete entity;
    }
    
    Entity* entity = nullptr;
};

// Test entity ID generation
TEST_F(EntityTest, IdGeneration) {
    Entity entity1;
    Entity entity2;
    
    // Ensure IDs are unique
    EXPECT_NE(entity1.getId(), entity2.getId());
}

// Test component addition and retrieval
TEST_F(EntityTest, ComponentAddAndGet) {
    // Create and add component
    MockComponent* mockComponent = new MockComponent();
    entity->addComponent(mockComponent);
    
    // Verify we can retrieve it
    Component* retrievedComponent = entity->getComponent<MockComponent>();
    EXPECT_EQ(mockComponent, retrievedComponent);
}

// Test component removal
TEST_F(EntityTest, ComponentRemoval) {
    // Create and add component
    MockComponent* mockComponent = new MockComponent();
    entity->addComponent(mockComponent);
    
    // Verify it's there
    EXPECT_EQ(mockComponent, entity->getComponent<MockComponent>());
    
    // Remove it
    entity->removeComponent<MockComponent>();
    
    // Verify it's gone
    EXPECT_EQ(nullptr, entity->getComponent<MockComponent>());
}

// Test component update
TEST_F(EntityTest, ComponentUpdate) {
    // Create mock component
    MockComponent* mockComponent = new MockComponent();
    
    // Set up expectation
    EXPECT_CALL(*mockComponent, update(::testing::_))
        .Times(1);
    
    // Add component
    entity->addComponent(mockComponent);
    
    // Update entity
    entity->update(0.16f);
}

// Test entity destruction with components
TEST_F(EntityTest, EntityDestruction) {
    // Create mock component
    MockComponent* mockComponent = new MockComponent();
    
    // Set up expectation for onDestroy
    EXPECT_CALL(*mockComponent, onDestroy())
        .Times(1);
    
    // Add component
    entity->addComponent(mockComponent);
    
    // Destroying the entity should trigger onDestroy for components
    delete entity;
    entity = nullptr; // Prevent double-delete in TearDown
}