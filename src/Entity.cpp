#include "Entity.h"
#include "Component.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "AIComponent.h"
#include "HealthComponent.h"
#include "InputComponent.h"
#include "ComponentUtils.h"
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

// Get all components in a vector
std::vector<Component*> Entity::getAllComponents() const {
    std::vector<Component*> result;
    result.reserve(m_components.size());
    
    for (const auto& [type, comp] : m_components) {
        result.push_back(comp.get());
    }
    
    return result;
}

// Subscript operator implementation for common component access by name
Component* Entity::operator[](const std::string& componentName) const {
    if (componentName == "transform" || componentName == "Transform") {
        return getComponent<Transform>();
    }
    else if (componentName == "physics" || componentName == "PhysicsComponent") {
        return getComponent<PhysicsComponent>();
    }
    else if (componentName == "render" || componentName == "RenderComponent") {
        return getComponent<RenderComponent>();
    }
    else if (componentName == "collision" || componentName == "CollisionComponent") {
        return getComponent<CollisionComponent>();
    }
    else if (componentName == "ai" || componentName == "AIComponent") {
        return getComponent<AIComponent>();
    }
    else if (componentName == "health" || componentName == "HealthComponent") {
        return getComponent<HealthComponent>();
    }
    else if (componentName == "input" || componentName == "InputComponent") {
        return getComponent<InputComponent>();
    }
    
    return nullptr;
}

// Stream insertion operator for Entity
std::ostream& operator<<(std::ostream& os, const Entity& entity) {
    os << "Entity(id=" << entity.m_id 
       << ", active=" << (entity.m_active ? "true" : "false")
       << ", components=[";
    
    // Print component names with their categories
    bool first = true;
    for (const auto& [type, comp] : entity.m_components) {
        if (!first) os << ", ";
        os << type.name();
        first = false;
    }
    
    os << "])";
    return os;
}