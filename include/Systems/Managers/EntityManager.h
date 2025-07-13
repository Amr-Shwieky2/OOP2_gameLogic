#pragma once
#include "Entity.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

class EntityManager {
public:
    using IdType = Entity::IdType;

    EntityManager();
    ~EntityManager();

    // Create a new entity (optional: accept a derived entity type)
    template <typename T = Entity, typename... Args>
    T* createEntity(Args&&... args);

    // Remove entity by ID
    void destroyEntity(IdType id);

    // Get entity by ID
    Entity* getEntity(IdType id);
    const Entity* getEntity(IdType id) const;

    // Update all entities
    void updateAll(float dt);

    // Remove all entities (danger: use carefully)
    void clear();

    // Apply function to all entities (e.g., for system queries)
    void forEach(const std::function<void(Entity*)>& func);

    // Get all current entities
    std::vector<Entity*> getAllEntities();
    // Add this method to EntityManager class
    void addEntity(std::unique_ptr<Entity> entity);
    void removeInactiveEntities();


private:
    std::unordered_map<IdType, std::unique_ptr<Entity>> m_entities;
    IdType m_nextId = 1;
};

template <typename T, typename... Args>
T* EntityManager::createEntity(Args&&... args) {
	static_assert(std::is_base_of<Entity, T>::value, "T must inherit from Entity");
	auto entity = std::make_unique<T>(std::forward<Args>(args)...);
	T* ptr = entity.get();
	m_entities[m_nextId++] = std::move(entity);
	return ptr;
}