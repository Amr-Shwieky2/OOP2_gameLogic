#include "CollisionAdapter.h"
#include "Entity.h"
#include <iostream>
#include <memory>

/**
 * Simple entity class for testing collisions
 */
class TestEntity : public Entity {
public:
    TestEntity(IdType id, const std::string& name) : Entity(id), m_name(name) {}
    const std::string& getName() const { return m_name; }
    
private:
    std::string m_name;
};

/**
 * Another test entity class
 */
class AnotherTestEntity : public Entity {
public:
    AnotherTestEntity(IdType id, const std::string& name) : Entity(id), m_name(name) {}
    const std::string& getName() const { return m_name; }
    
private:
    std::string m_name;
};

/**
 * Demonstration of using the CollisionAdapter with the existing collision system
 */
void testCollisionAdapter() {
    std::cout << "\n===== COLLISION ADAPTER TEST =====\n";
    
    // Create a collision adapter (which uses the MultiMethodDispatcher internally)
    CollisionAdapter adapter;
    
    // Register some handlers
    adapter.registerHandler<TestEntity, TestEntity>(
        [](TestEntity& e1, TestEntity& e2) {
            std::cout << "Test collision between " << e1.getName() 
                      << " and " << e2.getName() << std::endl;
        }
    );
    
    adapter.registerHandler<TestEntity, AnotherTestEntity>(
        [](TestEntity& e1, AnotherTestEntity& e2) {
            std::cout << "Collision between " << e1.getName() 
                      << " and " << e2.getName() << std::endl;
        }
    );
    
    // Create some test entities
    TestEntity entity1(1, "Entity 1");
    TestEntity entity2(2, "Entity 2");
    AnotherTestEntity entity3(3, "Entity 3");
    
    // Test the processCollision method
    adapter.processCollision(entity1, entity2);
    adapter.processCollision(entity1, entity3);
    
    // Test with the MultiMethodDispatcher directly
    MultiMethodDispatcher& dispatcher = MultiMethodDispatcher::getInstance();
    bool result = dispatcher.dispatch(entity1, entity2, "collision");
    std::cout << "Dispatcher result: " << (result ? "Handler found" : "No handler found") << std::endl;
    
    // Show the number of registered handlers
    std::cout << "Handler count: " << adapter.getHandlerCount() << std::endl;
    
    // Debug print the handlers
    std::cout << "Registered handlers:" << std::endl;
    adapter.debugPrintHandlers();
    
    std::cout << "===== COLLISION ADAPTER TEST COMPLETE =====\n";
}

// Expose this function for calling from the main demo
void runCollisionAdapterTest() {
    testCollisionAdapter();
}