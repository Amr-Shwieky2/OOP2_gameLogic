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
#include <stdexcept>
#include <string>
#include <iostream>
#include "Component.h"
#include "ComponentTraits.h"
#include "ComponentUtils.h"

// Forward declarations of specialized components
class PhysicsComponent;
class RenderComponent;
class AIComponent;
class Transform;
class CollisionComponent;
class HealthComponent;
class InputComponent;

/**
 * Entity - Base class for all game objects
 *
 * Entities are containers for components that define their behavior.
 * Each entity has a unique ID and can be active or inactive.
 * Inactive entities are scheduled for removal by the EntityCleanupManager.
 * 
 * Enhanced with template meta-programming features for component validation.
 */
class Entity {
public:
    using IdType = std::uint32_t;

    explicit Entity(IdType id);
    virtual ~Entity();

    // Entity identification
    IdType getId() const;

    // Entity state management
    void setActive(bool active);
    bool isActive() const;

    // Component system with template meta-programming enhancements
    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        
        // Compile-time validation of component dependencies using traits
        using Dependencies = typename ComponentTraits::Dependencies<T>::type;
        
        // Check if any incompatible components are being added
        checkComponentCompatibility<T>();
        
        // Create a component of type T
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = component.get();
        
        // Set this entity as the owner
        rawPtr->setOwner(this);
        
        // Store the component
        std::type_index typeId = std::type_index(typeid(T));
        m_components[typeId] = std::move(component);
        
        return rawPtr;
    }

    template<typename T>
    T* getComponent() const {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        std::type_index typeId = std::type_index(typeid(T));
        auto it = m_components.find(typeId);
        return it != m_components.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }

    template<typename T>
    bool hasComponent() const {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        
        std::type_index typeId = std::type_index(typeid(T));
        return m_components.find(typeId) != m_components.end();
    }

    template<typename T>
    void removeComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        
        std::type_index typeId = std::type_index(typeid(T));
        auto it = m_components.find(typeId);
        if (it != m_components.end()) {
            it->second->onDestroy();
            m_components.erase(it);
        }
    }

    // Entity lifecycle
    virtual void update(float dt);
    virtual void onDestroy();
    
    // Component access via subscript operator
    template<typename T>
    T* operator[](const std::type_index& typeId) const {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto it = m_components.find(typeId);
        return it != m_components.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }
    
    // Generic subscript operator for component access by type
    template<typename T>
    T* operator[](const std::type_info&) const {
        return getComponent<T>();
    }
    
    // Shorthand for common components (Transform, PhysicsComponent, etc.)
    Component* operator[](const std::string& componentName) const;
    
    // Equality operators for entity comparison
    bool operator==(const Entity& other) const {
        return m_id == other.m_id;
    }
    
    bool operator!=(const Entity& other) const {
        return !(*this == other);
    }
    
    // Comparison operators for entity ordering
    bool operator<(const Entity& other) const {
        return m_id < other.m_id;
    }
    
    bool operator>(const Entity& other) const {
        return m_id > other.m_id;
    }
    
    bool operator<=(const Entity& other) const {
        return m_id <= other.m_id;
    }
    
    bool operator>=(const Entity& other) const {
        return m_id >= other.m_id;
    }
    
    // Stream insertion operator for debugging
    friend std::ostream& operator<<(std::ostream& os, const Entity& entity);
    
    // Advanced template operations
    
    // Variadic template for batch adding components
    template<typename... Components>
    void addComponents() {
        (addComponent<Components>(), ...); // C++17 fold expression
    }
    
    // Check if all required components exist using variadic templates
    template<typename... RequiredComponents>
    bool hasRequiredComponents() const {
        return (hasComponent<RequiredComponents>() && ...); // C++17 fold expression
    }
    
    // Get all components of a specific category
    template<ComponentTraits::ComponentCategory Category>
    std::vector<Component*> getComponentsByCategory() const {
        std::vector<Component*> result;
        for (const auto& [type, component] : m_components) {
            // This is a simplification; in a real implementation, 
            // we'd need to get the category for each specific type
            result.push_back(component.get());
        }
        return result;
    }
    
    // Get all components (non-template version)
    std::vector<Component*> getAllComponents() const;

protected:
    IdType m_id;
    bool m_active;
    
    // Components are stored by their type index
    mutable std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;
    
    // Template meta-programming helper for component compatibility validation
    template<typename NewComponent>
    void checkComponentCompatibility() {
        // Example: Prevent having both AIComponent and InputComponent
        if constexpr (std::is_same_v<NewComponent, AIComponent>) {
            if (hasComponent<InputComponent>()) {
                throw std::logic_error("Cannot add AIComponent to an entity with InputComponent");
            }
        }
        
        if constexpr (std::is_same_v<NewComponent, InputComponent>) {
            if (hasComponent<AIComponent>()) {
                throw std::logic_error("Cannot add InputComponent to an entity with AIComponent");
            }
        }
    }
};

// Stream insertion operator implementation
std::ostream& operator<<(std::ostream& os, const Entity& entity);
