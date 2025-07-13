/**
 * @file Entity.h
 * @brief Declaration of the core Entity class used by the game engine.
 */
#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <vector>
#include <cstdint>
#include "Component.h"

class Component;

/**
 * @brief Base class for all game entities.
 *
 * An Entity acts as a container for a set of components that implement
 * the entity's behaviour. Components can be added or removed at runtime
 * and are indexed by their type. Each entity also keeps track of whether
 * it is currently active so the EntityManager can skip updating it when
 * needed.
 */
class Entity {
public:
    /** @brief Type used for unique entity identifiers. */
    using IdType = std::uint32_t;

    /**
     * @brief Construct a new Entity.
     * @param id Unique identifier assigned by the EntityManager.
     */
    explicit Entity(IdType id);

    /// Virtual destructor to allow polymorphic deletion.
    virtual ~Entity();

    /** @brief Get the unique identifier of this entity. */
    IdType getId() const;

    /**
     * @brief Add a component of type T to this entity.
     * @tparam T Component type derived from Component.
     * @tparam Args Parameter pack forwarded to the component constructor.
     * @param args Arguments used to construct the component.
     * @return Pointer to the newly created component instance.
     */
    template <typename T, typename... Args>
    T* addComponent(Args&&... args);

    /**
     * @brief Get a pointer to the component of type T.
     * @tparam T Component type.
     * @return Pointer to the component or nullptr if none exists.
     */
    template <typename T>
    T* getComponent() const;

    /**
     * @brief Determine whether this entity has a component of type T.
     * @tparam T Component type.
     * @return True if the component exists.
     */
    template <typename T>
    bool hasComponent() const;

    /**
     * @brief Remove the component of type T from the entity.
     * @tparam T Component type to remove.
     */
    template <typename T>
    void removeComponent();

    /**
     * @brief Set whether this entity is active.
     * @param active True to update and render the entity, false to disable it.
     */
    void setActive(bool active);

    /** @brief Check if the entity is currently active. */
    bool isActive() const;

    /**
     * @brief Update the entity and its components.
     * @param dt Time elapsed since the last update in seconds.
     */
    virtual void update(float dt);

    /** @brief Hook called when the entity is about to be removed. */
    virtual void onDestroy();

    /**
     * @brief Called when the entity dies.
     * @param killer Entity responsible for killing this entity. May be nullptr.
     *
     * The default implementation does nothing; subclasses can override
     * this to perform custom death behaviour.
     */
    virtual void onDeath([[maybe_unused]] Entity* killer) {
        // Default implementation does nothing
        // Derived classes can override for special death behavior
    }

protected:
    /// Unique identifier assigned to this entity.
    IdType m_id;

    /// Flag used by the EntityManager to skip updates when false.
    bool m_active = true;

    /// Storage for all components owned by the entity keyed by type.
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
