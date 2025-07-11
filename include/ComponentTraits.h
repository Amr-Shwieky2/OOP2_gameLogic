#pragma once
#include <type_traits>
#include <tuple>
#include "ForwardDeclarations.h"

/**
 * ComponentTraits - Template metaprogramming system to categorize components
 * 
 * This system allows compile-time categorization of components into different 
 * types: Visual, Logic, Physics, etc. These traits can be used for validation,
 * dependency resolution, and optimization.
 */
namespace ComponentTraits {

    // Component categories
    enum class ComponentCategory {
        None       = 0,
        Visual     = 1 << 0,  // Components related to rendering (RenderComponent, AnimationComponent)
        Logic      = 1 << 1,  // Components related to game logic (AIComponent, HealthComponent)
        Physics    = 1 << 2,  // Components related to physics simulation (PhysicsComponent, CollisionComponent)
        Input      = 1 << 3,  // Components related to input handling (InputComponent)
        Sound      = 1 << 4,  // Components related to audio (AudioComponent)
        Transform  = 1 << 5,  // Components related to spatial transforms (Transform)
        Network    = 1 << 6,  // Components related to networking
        Data       = 1 << 7,  // Components primarily storing data
        System     = 1 << 8   // Components that manage system resources or act as interfaces
    };
    
    // Allow bitwise operations on ComponentCategory for complex categorization
    inline ComponentCategory operator|(ComponentCategory a, ComponentCategory b) {
        return static_cast<ComponentCategory>(static_cast<int>(a) | static_cast<int>(b));
    }
    
    inline ComponentCategory operator&(ComponentCategory a, ComponentCategory b) {
        return static_cast<ComponentCategory>(static_cast<int>(a) & static_cast<int>(b));
    }
    
    inline bool hasCategory(ComponentCategory value, ComponentCategory category) {
        return static_cast<int>(value & category) != 0;
    }

    // Primary trait template - default is None
    template<typename T>
    struct ComponentCategoryTrait {
        static constexpr ComponentCategory value = ComponentCategory::None;
    };
    
    // Compatibility traits - determine if components can work together
    template<typename T, typename U>
    struct AreCompatible : std::true_type {};
    
    // Dependency traits - determine if one component depends on another
    template<typename T>
    struct Dependencies {
        // By default, no dependencies
        using type = std::tuple<>;
    };
    
    // Helper to check if a component has a specific dependency
    template<typename Component, typename Dependency>
    struct HasDependency : std::false_type {};
    
    // Implementation using variadic templates and parameter packs
    template<typename Component, typename... Dependencies>
    struct HasDependencyImpl;
    
    template<typename Component, typename FirstDep, typename... RestDeps>
    struct HasDependencyImpl<Component, FirstDep, RestDeps...> {
        static constexpr bool value = 
            std::is_same_v<Component, FirstDep> || 
            HasDependencyImpl<Component, RestDeps...>::value;
    };
    
    template<typename Component>
    struct HasDependencyImpl<Component> {
        static constexpr bool value = false;
    };
    
    // Initialization trait - checks if a component needs special initialization
    template<typename T>
    struct NeedsInitialization : std::false_type {};
    
    // Serialization trait - checks if a component is serializable
    template<typename T>
    struct IsSerializable : std::false_type {};
    
    // Networking trait - checks if a component needs to be synchronized over network
    template<typename T>
    struct NeedsNetworkSync : std::false_type {};
    
    // Performance impact trait - estimates the computational cost of a component (0-10)
    template<typename T>
    struct PerformanceImpact {
        static constexpr int value = 1; // Default is low impact
    };
    
    // Validation methods to enforce constraints at compile time
    
    // Check if components are compatible
    template<typename T, typename U>
    constexpr bool areCompatible() {
        return AreCompatible<T, U>::value;
    }
    
    // Check if a component has a specific category
    template<typename T>
    constexpr bool hasCategory(ComponentCategory category) {
        return hasCategory(ComponentCategoryTrait<T>::value, category);
    }
    
    // Get component category
    template<typename T>
    constexpr ComponentCategory getCategory() {
        return ComponentCategoryTrait<T>::value;
    }
    
    // Check if component needs initialization
    template<typename T>
    constexpr bool needsInitialization() {
        return NeedsInitialization<T>::value;
    }
    
    // Check if component is serializable
    template<typename T>
    constexpr bool isSerializable() {
        return IsSerializable<T>::value;
    }
    
    // Get performance impact of a component
    template<typename T>
    constexpr int getPerformanceImpact() {
        return PerformanceImpact<T>::value;
    }
}

// Include specializations for all component types
#include "ComponentTraitSpecializations.h"