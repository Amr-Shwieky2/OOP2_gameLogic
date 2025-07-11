#pragma once
#include <type_traits>
#include "Entity.h"
#include "ComponentTraits.h"
#include "ForwardDeclarations.h"

/**
 * ComponentSFINAE - Utilities for conditional method compilation based on component presence
 * 
 * This file contains SFINAE (Substitution Failure Is Not An Error) templates that
 * enable conditional method compilation based on the presence of specific components.
 */
namespace ComponentSFINAE {

    // SFINAE helper to enable methods only if an entity has specific components
    template<typename... RequiredComponents>
    struct EnableIfHasComponents {
        template<typename EntityType>
        using type = std::enable_if_t<
            std::is_base_of_v<Entity, std::remove_pointer_t<EntityType>> &&
            std::remove_pointer_t<EntityType>::template hasRequiredComponents<RequiredComponents...>()
        >;
    };
    
    // Helper for methods that should be enabled only if an entity has a specific component
    template<typename RequiredComponent>
    struct EnableIfHasComponent {
        template<typename EntityType>
        using type = typename EnableIfHasComponents<RequiredComponent>::template type<EntityType>;
    };
    
    // Helper for enabling/disabling methods based on component category
    template<ComponentTraits::ComponentCategory Category>
    struct EnableIfHasComponentCategory {
        template<typename ComponentType>
        using type = std::enable_if_t<
            ComponentTraits::hasCategory<ComponentType>(Category)
        >;
    };
    
    // Helper to detect if a component has a specific method
    template<typename T, typename = void>
    struct HasUpdate : std::false_type {};
    
    template<typename T>
    struct HasUpdate<T, std::void_t<decltype(std::declval<T>().update(0.0f))>> : std::true_type {};
    
    template<typename T, typename = void>
    struct HasRender : std::false_type {};
    
    template<typename T>
    struct HasRender<T, std::void_t<decltype(std::declval<T>().render(std::declval<sf::RenderTarget&>()))>> : std::true_type {};
    
    template<typename T, typename = void>
    struct HasInitialize : std::false_type {};
    
    template<typename T>
    struct HasInitialize<T, std::void_t<decltype(std::declval<T>().initialize())>> : std::true_type {};
    
    // SFINAE helpers for conditional method calls
    
    // Call update only if the component has an update method
    template<typename T>
    inline void callUpdate(T& component, float dt, std::enable_if_t<HasUpdate<T>::value, int> = 0) {
        component.update(dt);
    }
    
    template<typename T>
    inline void callUpdate(T& component, float dt, std::enable_if_t<!HasUpdate<T>::value, int> = 0) {
        // Do nothing - component doesn't have update method
    }
    
    // Call render only if the component has a render method
    template<typename T, typename RenderTarget>
    inline void callRender(T& component, RenderTarget& target, std::enable_if_t<HasRender<T>::value, int> = 0) {
        component.render(target);
    }
    
    template<typename T, typename RenderTarget>
    inline void callRender(T& component, RenderTarget& target, std::enable_if_t<!HasRender<T>::value, int> = 0) {
        // Do nothing - component doesn't have render method
    }
    
    // Call initialize only if the component has an initialize method
    template<typename T>
    inline bool callInitialize(T& component, std::enable_if_t<HasInitialize<T>::value, int> = 0) {
        return component.initialize();
    }
    
    template<typename T>
    inline bool callInitialize(T& component, std::enable_if_t<!HasInitialize<T>::value, int> = 0) {
        return true; // No initialization needed, so it's always "successful"
    }
}

// Convenience macros for SFINAE method declarations

// Enable a method only if the entity has the specified components
#define ENABLE_IF_HAS_COMPONENTS(EntityType, ...) \
    typename ComponentSFINAE::EnableIfHasComponents<__VA_ARGS__>::template type<EntityType>* = nullptr

// Enable a method only if the entity has a specific component
#define ENABLE_IF_HAS_COMPONENT(EntityType, Component) \
    typename ComponentSFINAE::EnableIfHasComponent<Component>::template type<EntityType>* = nullptr

// Usage example:
/*
class MySystem {
public:
    // This method is only enabled if the entity has both Transform and RenderComponent
    template<typename EntityType>
    void render(EntityType* entity, ENABLE_IF_HAS_COMPONENTS(EntityType, Transform, RenderComponent)) {
        // Safe to use getComponent<Transform> and getComponent<RenderComponent> here
        auto transform = entity->template getComponent<Transform>();
        auto render = entity->template getComponent<RenderComponent>();
        
        // Method body...
    }
    
    // This method is only enabled if the component belongs to the Visual category
    template<typename ComponentType>
    void updateVisual(ComponentType* component, 
                     typename ComponentSFINAE::EnableIfHasComponentCategory<
                        ComponentTraits::ComponentCategory::Visual
                     >::template type<ComponentType>* = nullptr) {
        // This will only be called for visual components
    }
};
*/