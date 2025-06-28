#include "Entity.h"
#include "Component.h"

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
    for (auto& [type, comp] : m_components) {
        comp->update(dt);
    }
}

void Entity::onDestroy() {
    for (auto& [type, comp] : m_components) {
        comp->onDestroy();
    }
}