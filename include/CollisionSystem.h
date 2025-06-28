#pragma once
#include <typeindex>
#include <map>
#include <utility>
#include <functional>
#include "Entity.h"

class CollisionSystem {
public:
    using CollisionFunc = void(*)(Entity&, Entity&);
    using CollisionKey = std::pair<std::type_index, std::type_index>;
    using CollisionMap = std::map<CollisionKey, CollisionFunc>;

    // Registers a collision handler for a pair of types
    template<typename A, typename B>
    void registerHandler(CollisionFunc func);

    // Processes a collision between two entities
    void processCollision(Entity& a, Entity& b);

    // Singleton pattern (optional, for global access)
    static CollisionSystem& instance();

private:
    CollisionMap m_handlers;

    // Helper to look up the handler
    CollisionFunc lookup(const std::type_index& a, const std::type_index& b) const;
};

// Template implementation
template<typename A, typename B>
void CollisionSystem::registerHandler(CollisionFunc func) {
    m_handlers[{typeid(A), typeid(B)}] = func;
}