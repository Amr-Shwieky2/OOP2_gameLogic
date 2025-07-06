#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <vector>
#include <cstdint>
#include "Component.h"

class Component;

class Entity {
public:
    using IdType = std::uint32_t;

    explicit Entity(IdType id);
    virtual ~Entity();

    IdType getId() const;

    template <typename T, typename... Args>
    T* addComponent(Args&&... args);

    template <typename T>
    T* getComponent() const;

    template <typename T>
    bool hasComponent() const;

    template <typename T>
    void removeComponent();

    void setActive(bool active);
    bool isActive() const;

    virtual void update(float dt);
    virtual void onDestroy();

    // NEW: Add virtual onDeath method for entities to override
    virtual void onDeath(Entity* killer) {
        // Default implementation does nothing
        // Derived classes can override for special death behavior
    }

protected:
    IdType m_id;
    bool m_active = true;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;
};

// Template implementations remain the same...
template <typename T, typename... Args>
T* Entity::addComponent(Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
    auto comp = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = comp.get();
    comp->setOwner(this);
    m_components[typeid(T)] = std::move(comp);
    return ptr;
}

template <typename T>
T* Entity::getComponent() const {
    auto it = m_components.find(typeid(T));
    if (it != m_components.end()) {
        return static_cast<T*>(it->second.get());
    }
    return nullptr;
}

template <typename T>
bool Entity::hasComponent() const {
    return m_components.find(typeid(T)) != m_components.end();
}

template <typename T>
void Entity::removeComponent() {
    m_components.erase(typeid(T));
}