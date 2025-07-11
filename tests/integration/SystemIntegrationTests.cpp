#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Entity.h"
#include "Component.h"
#include "AIComponent.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "Memory/MemoryManager.h"
#include "Memory/MemoryLeakDetector.h"
#include "Performance/Profiler.h"
#include "MultiMethodDispatcher.h"
#include <memory>

// Integration test for multiple systems working together
class SystemIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Enable profiling and memory tracking
        MemoryLeakDetector::getInstance().setEnabled(true);
        Performance::ProfilingManager::getInstance().setEnabled(true);
        Performance::ProfilingManager::getInstance().resetMetrics();
        
        // Initialize core systems
        memoryManager = &MemoryManager::getInstance();
        dispatcher = &MultiMethodDispatcher::getInstance();
        
        // Create a test entity
        entity = std::make_unique<Entity>();
    }
    
    void TearDown() override {
        // Clean up entity before disabling memory tracking
        entity.reset();
        
        // Check for memory leaks
        auto& leakDetector = MemoryLeakDetector::getInstance();
        EXPECT_FALSE(leakDetector.hasLeaks()) << "Memory leaks detected!";
        
        // Disable systems
        leakDetector.setEnabled(false);
        Performance::ProfilingManager::getInstance().setEnabled(false);
    }
    
    // Common test components
    class TestRenderComponent : public RenderComponent {
    public:
        MOCK_METHOD(void, draw, (sf::RenderTarget&), (override));
        MOCK_METHOD(void, update, (float), (override));
    };
    
    class TestPhysicsComponent : public PhysicsComponent {
    public:
        MOCK_METHOD(void, update, (float), (override));
        MOCK_METHOD(void, applyForce, (const sf::Vector2f&), (override));
    };
    
    class TestAIComponent : public AIComponent {
    public:
        explicit TestAIComponent(AIStrategy* strategy = nullptr) : AIComponent(strategy) {}
        MOCK_METHOD(void, update, (float), (override));
        
        using AIComponent::setStrategy;
    };
    
    // Systems to test
    MemoryManager* memoryManager = nullptr;
    MultiMethodDispatcher* dispatcher = nullptr;
    std::unique_ptr<Entity> entity;
};

// Test entity-component integration
TEST_F(SystemIntegrationTest, EntityComponentIntegration) {
    // Create components
    auto physicsComponent = new TestPhysicsComponent();
    auto renderComponent = new TestRenderComponent();
    auto aiComponent = new TestAIComponent();
    
    // Set up expectations
    EXPECT_CALL(*physicsComponent, update(::testing::_))
        .Times(1);
    EXPECT_CALL(*renderComponent, update(::testing::_))
        .Times(1);
    EXPECT_CALL(*aiComponent, update(::testing::_))
        .Times(1);
    
    // Add components to entity
    entity->addComponent(physicsComponent);
    entity->addComponent(renderComponent);
    entity->addComponent(aiComponent);
    
    // Update entity (should update all components)
    entity->update(0.16f);
    
    // Verify component retrieval
    EXPECT_EQ(physicsComponent, entity->getComponent<PhysicsComponent>());
    EXPECT_EQ(renderComponent, entity->getComponent<RenderComponent>());
    EXPECT_EQ(aiComponent, entity->getComponent<AIComponent>());
}

// Test integration between MultiMethodDispatcher and entity collision
TEST_F(SystemIntegrationTest, CollisionIntegration) {
    // Create test entities
    auto player = std::make_unique<Entity>();
    auto enemy = std::make_unique<Entity>();
    
    // Track collision
    bool collisionHandled = false;
    
    // Register collision handler
    dispatcher->registerHandler<Entity, Entity>("collision",
        [&collisionHandled](Entity& entity1, Entity& entity2) {
            collisionHandled = true;
        }
    );
    
    // Dispatch collision event
    dispatcher->dispatch(*player, *enemy, "collision");
    
    // Verify collision was handled
    EXPECT_TRUE(collisionHandled);
}

// Test memory management integration
TEST_F(SystemIntegrationTest, MemoryManagementIntegration) {
    // Enable memory tracking for this scope
    Performance::ScopedProfile memoryProfile("MemoryAllocation");
    
    // Create a pool for TestRenderComponent
    constexpr size_t POOL_SIZE = 10;
    auto& pool = memoryManager->getPool<TestRenderComponent, sizeof(TestRenderComponent), POOL_SIZE>();
    
    // Track pool allocations
    size_t initialAllocationCount = pool.getAllocatedCount();
    
    // Allocate objects from the pool
    std::vector<TestRenderComponent*> components;
    for (size_t i = 0; i < 5; ++i) {
        auto* component = pool.allocate();
        EXPECT_NE(nullptr, component);
        components.push_back(component);
    }
    
    // Verify allocation count
    EXPECT_EQ(initialAllocationCount + 5, pool.getAllocatedCount());
    
    // Deallocate objects
    for (auto* component : components) {
        pool.deallocate(component);
    }
    
    // Verify allocation count is back to initial value
    EXPECT_EQ(initialAllocationCount, pool.getAllocatedCount());
}

// Test component dependency resolution
TEST_F(SystemIntegrationTest, ComponentDependencyIntegration) {
    // Add physics component first
    auto physicsComponent = new PhysicsComponent();
    entity->addComponent(physicsComponent);
    
    // Add render component that depends on physics component
    auto renderComponent = new RenderComponent();
    entity->addComponent(renderComponent);
    
    // Verify render component can access physics component
    auto retrievedPhysics = renderComponent->getOwner()->getComponent<PhysicsComponent>();
    EXPECT_EQ(physicsComponent, retrievedPhysics);
}

// Test AI strategy integration with AIComponent
TEST_F(SystemIntegrationTest, AIStrategyIntegration) {
    // Create a mock AI strategy
    class MockAIStrategy : public AIStrategy {
    public:
        MOCK_METHOD(void, update, (float), (override));
        MOCK_METHOD(const char*, getName, (), (const, override));
    };
    
    // Create the mock strategy
    auto mockStrategy = new MockAIStrategy();
    EXPECT_CALL(*mockStrategy, getName())
        .WillRepeatedly(::testing::Return("MockStrategy"));
    
    // Expect the strategy's update to be called
    EXPECT_CALL(*mockStrategy, update(::testing::_))
        .Times(1);
    
    // Create AI component with the strategy
    auto aiComponent = new TestAIComponent(mockStrategy);
    entity->addComponent(aiComponent);
    
    // Update the entity
    entity->update(0.16f);
    
    // Change strategy
    auto newStrategy = new MockAIStrategy();
    EXPECT_CALL(*newStrategy, getName())
        .WillRepeatedly(::testing::Return("NewMockStrategy"));
    
    // Expect the new strategy's update to be called
    EXPECT_CALL(*newStrategy, update(::testing::_))
        .Times(1);
        
    // Switch strategy
    aiComponent->setStrategy(newStrategy);
    
    // Update entity again
    entity->update(0.16f);
}

// Test performance profiling integration with game systems
TEST_F(SystemIntegrationTest, PerformanceProfilingIntegration) {
    auto& profiler = Performance::ProfilingManager::getInstance();
    profiler.resetMetrics();
    
    // Profile entity update
    {
        Performance::ScopedProfile profile("EntityUpdate");
        
        // Create components
        auto physicsComponent = new TestPhysicsComponent();
        auto renderComponent = new TestRenderComponent();
        
        // Allow these calls without expectations since we're focusing on profiling
        ON_CALL(*physicsComponent, update(::testing::_))
            .WillByDefault(::testing::Return());
        ON_CALL(*renderComponent, update(::testing::_))
            .WillByDefault(::testing::Return());
        
        // Add components
        entity->addComponent(physicsComponent);
        entity->addComponent(renderComponent);
        
        // Update entity
        entity->update(0.16f);
    }
    
    // Verify profile metrics were captured
    const auto& metrics = profiler.getMetrics();
    auto it = metrics.find("EntityUpdate");
    ASSERT_NE(metrics.end(), it);
    EXPECT_EQ(1, it->second.callCount);
    EXPECT_GT(it->second.totalTime, 0.0);
}

// Test game entity destruction and cleanup
TEST_F(SystemIntegrationTest, EntityDestructionIntegration) {
    // Create components
    auto physicsComponent = new TestPhysicsComponent();
    auto renderComponent = new TestRenderComponent();
    auto aiComponent = new TestAIComponent();
    
    // Setup component destruction expectations
    EXPECT_CALL(*physicsComponent, update(::testing::_))
        .Times(::testing::AtMost(1));
    EXPECT_CALL(*renderComponent, update(::testing::_))
        .Times(::testing::AtMost(1));
    EXPECT_CALL(*aiComponent, update(::testing::_))
        .Times(::testing::AtMost(1));
    
    // Add components to entity
    entity->addComponent(physicsComponent);
    entity->addComponent(renderComponent);
    entity->addComponent(aiComponent);
    
    // Destroy entity and verify proper cleanup
    size_t initialMemory = memoryManager->getTotalMemoryUsage();
    entity.reset();
    size_t finalMemory = memoryManager->getTotalMemoryUsage();
    
    // Memory should be the same or less after entity destruction
    EXPECT_LE(finalMemory, initialMemory);
}