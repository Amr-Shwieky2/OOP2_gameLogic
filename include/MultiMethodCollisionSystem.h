#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <utility>
#include <memory>
#include "Entity.h"

/**
 * MultiMethodCollisionSystem - Implements multimethods for collision handling
 *
 * This system uses a hash table to store collision handlers based on type pairs.
 * It allows for dynamic dispatch of collision handling functions without
 * modifying the entity classes (Open/Closed Principle).
 *
 * Based on the course material about vtbl and multimethods implementation.
 */
class MultiMethodCollisionSystem {
public:
    // Type alias for collision handler function
    using CollisionHandler = std::function<void(Entity&, Entity&)>;

    // Key type for the collision map (pair of type indices)
    using CollisionKey = std::pair<std::type_index, std::type_index>;

    // Hash function for the collision key
    struct CollisionKeyHash {
        std::size_t operator()(const CollisionKey& key) const {
            // Combine hashes of both types
            auto h1 = std::hash<std::type_index>{}(key.first);
            auto h2 = std::hash<std::type_index>{}(key.second);
            // Use bit shifting for better distribution
            return h1 ^ (h2 << 1);
        }
    };

    // The collision handler map
    using CollisionMap = std::unordered_map<CollisionKey, CollisionHandler, CollisionKeyHash>;

    /**
     * Register a collision handler for specific entity types
     *
     * @tparam T1 First entity type (must inherit from Entity)
     * @tparam T2 Second entity type (must inherit from Entity)
     * @param handler Function to handle collision between T1 and T2
     *
     * Example usage:
     *   system.registerHandler<PlayerEntity, CoinEntity>(
     *       [](PlayerEntity& player, CoinEntity& coin) {
     *           player.addScore(coin.getValue());
     *           coin.collect();
     *       }
     *   );
     */
    template<typename T1, typename T2>
    void registerHandler(std::function<void(T1&, T2&)> handler) {
        static_assert(std::is_base_of<Entity, T1>::value, "T1 must inherit from Entity");
        static_assert(std::is_base_of<Entity, T2>::value, "T2 must inherit from Entity");

        // Create key from type indices
        CollisionKey key = makeKey<T1, T2>();

        // Wrap the handler to cast from Entity& to specific types
        m_handlers[key] = [handler](Entity& e1, Entity& e2) {
            // Safe cast - we know the types match because of how we store them
            T1& entity1 = static_cast<T1&>(e1);
            T2& entity2 = static_cast<T2&>(e2);
            handler(entity1, entity2);
            };
    }

    /**
     * Process collision between two entities
     *
     * Looks up the appropriate handler based on runtime types and calls it.
     * If no handler is registered for the type pair, nothing happens.
     * Automatically handles both orderings (A,B) and (B,A).
     *
     * @param entity1 First entity in the collision
     * @param entity2 Second entity in the collision
     * @return true if a handler was found and executed, false otherwise
     */
    bool processCollision(Entity& entity1, Entity& entity2) {
        // Get runtime type information
        std::type_index type1 = std::type_index(typeid(entity1));
        std::type_index type2 = std::type_index(typeid(entity2));

        // Try first ordering (entity1, entity2)
        CollisionKey key{ type1, type2 };
        auto it = m_handlers.find(key);

        if (it != m_handlers.end()) {
            it->second(entity1, entity2);
            return true;
        }

        // Try reverse ordering (entity2, entity1)
        CollisionKey reverseKey{ type2, type1 };
        it = m_handlers.find(reverseKey);

        if (it != m_handlers.end()) {
            // Call with reversed parameters
            it->second(entity2, entity1);
            return true;
        }

        // No handler found
        return false;
    }

    /**
     * Clear all registered handlers
     */
    void clear() {
        m_handlers.clear();
    }

    /**
     * Get the number of registered handlers
     */
    size_t getHandlerCount() const {
        return m_handlers.size();
    }

    /**
     * Check if a handler exists for a specific type pair
     */
    template<typename T1, typename T2>
    bool hasHandler() const {
        CollisionKey key = makeKey<T1, T2>();
        return m_handlers.find(key) != m_handlers.end() ||
            m_handlers.find({ key.second, key.first }) != m_handlers.end();
    }
    void debugPrintHandlers() const;

private:
    // The main collision handler storage
    CollisionMap m_handlers;

    /**
     * Helper to create a collision key from template types
     */
    template<typename T1, typename T2>
    static CollisionKey makeKey() {
        return { std::type_index(typeid(T1)), std::type_index(typeid(T2)) };
    }
};