#include "Entity.h"
#include "Component.h"
#include <EnemyEntity.h>
#include <iostream>

Entity::Entity(IdType id)
    : m_id(id), m_active(true) {
}

Entity::~Entity() {}

Entity::IdType Entity::getId() const {
    return m_id;
}

void Entity::setActive(bool active) {
    m_active = active;
}

bool Entity::isActive() const {
    return m_active;
}

void Entity::update(float dt) {
    // Debug for enemies only
    static int updateCount = 0;
    updateCount++;

    if (dynamic_cast<EnemyEntity*>(this) && updateCount % 60 == 0) {
        std::cout << "[ENTITY] Updating entity " << m_id
            << " Components: " << m_components.size() << std::endl;
    }

    for (auto& [type, comp] : m_components) {
        if (comp) {
            comp->update(dt);
        }
    }
}

void Entity::onDestroy() {
    for (auto& [type, comp] : m_components) {
        comp->onDestroy();
    }
}