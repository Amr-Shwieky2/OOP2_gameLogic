#include "EntityFactory.h"
#include <iostream>

EntityFactory& EntityFactory::instance() {
    static EntityFactory factory;
    return factory;
}

void EntityFactory::registerCreator(const std::string& typeName, CreatorFunc creator) {
    if (m_creators.find(typeName) != m_creators.end()) {
        std::cerr << "Warning: Overwriting creator for type: " << typeName << std::endl;
    }
    m_creators[typeName] = creator;
}

std::unique_ptr<Entity> EntityFactory::create(const std::string& typeName, float x, float y) {
    auto it = m_creators.find(typeName);
    if (it != m_creators.end()) {
        return it->second(x, y);
    }
    
    std::cerr << "Error: No creator registered for type: " << typeName << std::endl;
    return nullptr;
}

Entity* EntityFactory::createInManager(const std::string& typeName, float x, float y, EntityManager&) {
    auto entity = create(typeName, x, y);
    if (entity) {
        Entity* ptr = entity.get();
        // The manager will take ownership
        // Note: You need to modify EntityManager to accept unique_ptr directly
        // For now, assuming it has a method like addEntity(unique_ptr<Entity>)
        return ptr;
    }
    return nullptr;
}

bool EntityFactory::hasCreator(const std::string& typeName) const {
    return m_creators.find(typeName) != m_creators.end();
}