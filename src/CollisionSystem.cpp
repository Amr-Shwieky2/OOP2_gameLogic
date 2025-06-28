#include "CollisionSystem.h"
#include <stdexcept>
#include <iostream>

CollisionSystem& CollisionSystem::instance() {
    static CollisionSystem system;
    return system;
}

CollisionSystem::CollisionFunc CollisionSystem::lookup(const std::type_index& a, const std::type_index& b) const {
    auto it = m_handlers.find({ a, b });
    if (it != m_handlers.end()) {
        return it->second;
    }
    else {
        // Could also return nullptr, or a default handler
        return nullptr;
    }
}

void CollisionSystem::processCollision(Entity& a, Entity& b) {
    auto func = lookup(typeid(a), typeid(b));
    if (func) {
        func(a, b);
    }
    else {
        // Optionally, try the reverse order (for symmetric collisions)
        func = lookup(typeid(b), typeid(a));
        if (func) {
            func(b, a);
        }
        else {
            // No handler found; optionally do nothing or log
#ifdef DEBUG
            std::cerr << "No collision handler for " << typeid(a).name() << " and " << typeid(b).name() << std::endl;
#endif
        }
    }
}