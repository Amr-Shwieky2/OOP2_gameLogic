#pragma once

#include "MultiMethodDispatcher.h"
#include "MultiMethodCollisionSystem.h"
#include "Entity.h"

/**
 * CollisionAdapter - Converts from the old collision system to the new MultiMethodDispatcher
 * 
 * This adapter class allows for a smooth transition from the old collision system
 * to the new generalized MultiMethodDispatcher. It implements the same interface
 * as the old MultiMethodCollisionSystem but delegates to the MultiMethodDispatcher.
 */
class CollisionAdapter : public MultiMethodCollisionSystem {
public:
    CollisionAdapter() {
        // Clear any existing handlers for the collision domain
        MultiMethodDispatcher::getInstance().clearDomain("collision");
    }
    
    // Register collision handler (using same template signature as base class)
    template<typename T1, typename T2>
    void registerHandler(std::function<void(T1&, T2&)> handler) {
        // Register with the global dispatcher using the collision domain
        MultiMethodDispatcher::getInstance().registerHandler<T1, T2>("collision", handler);
    }
    
    // Process a collision between two entities
    bool processCollision(Entity& entity1, Entity& entity2) override {
        // Delegate to the global dispatcher
        return MultiMethodDispatcher::getInstance().dispatch(entity1, entity2, "collision");
    }
    
    // Clear all registered handlers
    void clear() override {
        // Clear just the collision domain in the global dispatcher
        MultiMethodDispatcher::getInstance().clearDomain("collision");
    }
    
    // Get the number of registered handlers
    size_t getHandlerCount() const override {
        // Get handler count for the collision domain
        return MultiMethodDispatcher::getInstance().getHandlerCount("collision");
    }
    
    // Check if a handler exists for a specific type pair
    template<typename T1, typename T2>
    bool hasHandler() const {
        // Check if a handler exists in the collision domain
        return MultiMethodDispatcher::getInstance().hasHandler<T1, T2>("collision");
    }
    
    // Debug print all registered handlers
    void debugPrintHandlers() const override {
        // Print handlers for the collision domain
        MultiMethodDispatcher::getInstance().debugPrintHandlers("collision");
    }
};