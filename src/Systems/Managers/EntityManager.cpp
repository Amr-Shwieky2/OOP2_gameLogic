#include "EntityManager.h"

EntityManager::EntityManager() = default;
EntityManager::~EntityManager() = default;

void EntityManager::destroyEntity(IdType id) {
    m_entities.erase(id);
}

Entity* EntityManager::getEntity(IdType id) {
    auto it = m_entities.find(id);
    return it != m_entities.end() ? it->second.get() : nullptr;
}

const Entity* EntityManager::getEntity(IdType id) const {
    auto it = m_entities.find(id);
    return it != m_entities.end() ? it->second.get() : nullptr;
}

void EntityManager::updateAll(float dt) {
    for (auto& [id, entity] : m_entities) {
        if (entity->isActive())
            entity->update(dt);
    }
}

void EntityManager::clear() {
    m_entities.clear();
}

void EntityManager::forEach(const std::function<void(Entity*)>& func) {
    for (auto& [id, entity] : m_entities) {
        func(entity.get());
    }
}

std::vector<Entity*> EntityManager::getAllEntities() {
    std::vector<Entity*> result;
    for (auto& [id, entity] : m_entities) {
        result.push_back(entity.get());
    }
    return result;
}

void EntityManager::addEntity(std::unique_ptr<Entity> entity)
{
    if (entity) {
        IdType id = entity->getId();
        m_entities[id] = std::move(entity);
    }
}

void EntityManager::removeInactiveEntities() {
    for (auto it = m_entities.begin(); it != m_entities.end(); ) {
        if (!it->second->isActive()) {
            it = m_entities.erase(it);
        }
        else {
            ++it;
        }
    }
}

EntityManager::IdType EntityManager::generateId() {
    return m_nextId++;
}
