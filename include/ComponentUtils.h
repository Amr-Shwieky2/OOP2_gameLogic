#pragma once
#include "ComponentTraits.h"
#include "Component.h"
#include <tuple>
#include <type_traits>

class Entity;

/**
 * ComponentUtils - Template utilities for component operations
 * 
 * This class provides compile-time and runtime validation for components,
 * dependency resolution, and batch operations on components.
 */
namespace ComponentUtils {

    // SFINAE helper to detect if a component has an initialize method
    template<typename T>
    class HasInitializeMethod {
        template<typename C> static auto test(int) -> decltype(std::declval<C>().initialize(), std::true_type());
        template<typename C> static std::false_type test(...);
    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };
    
    // SFINAE helper to detect if a component has a validateDependencies method
    template<typename T>
    class HasValidateDependenciesMethod {
        template<typename C> static auto test(int) -> decltype(std::declval<C>().validateDependencies(), std::true_type());
        template<typename C> static std::false_type test(...);
    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };
    
    // SFINAE helper to detect if a component has a synchronizeWithTransform method
    template<typename T>
    class HasSynchronizeMethod {
        template<typename C> static auto test(int) -> decltype(std::declval<C>().synchronizeWithTransform(), std::true_type());
        template<typename C> static std::false_type test(...);
    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    // -------- Dependency Resolution Helpers --------
    
    // Helper to check if dependencies are satisfied
    template<typename T, typename... Dependencies>
    struct AreDependenciesSatisfied;
    
    template<typename T>
    struct AreDependenciesSatisfied<T> {
        static bool check(Entity* entity) {
            return true; // No dependencies to check
        }
    };
    
    template<typename T, typename FirstDep, typename... RestDeps>
    struct AreDependenciesSatisfied<T, FirstDep, RestDeps...> {
        static bool check(Entity* entity);
    };
    
    // Variadic template for initializing components
    template<typename... Components>
    struct BatchInitializer;
    
    template<>
    struct BatchInitializer<> {
        static void initialize(Entity* entity) {
            // Base case - nothing to initialize
        }
    };
    
    template<typename FirstComponent, typename... RestComponents>
    struct BatchInitializer<FirstComponent, RestComponents...> {
        static void initialize(Entity* entity);
    };
    
    // Variadic template for synchronizing components
    template<typename... Components>
    struct BatchSynchronizer;
    
    template<>
    struct BatchSynchronizer<> {
        static void synchronize(Entity* entity) {
            // Base case - nothing to synchronize
        }
    };
    
    template<typename FirstComponent, typename... RestComponents>
    struct BatchSynchronizer<FirstComponent, RestComponents...> {
        static void synchronize(Entity* entity);
    };
    
    // -------- Public Interface --------
    
    // Check if a component can be added to an entity
    template<typename T>
    bool canAddComponent(Entity* entity);
    
    // Initialize component with dependency checking
    template<typename T>
    bool initializeComponent(T* component, Entity* entity);
    
    // Check if a component has all required dependencies
    template<typename T>
    bool validateDependencies(T* component, Entity* entity);
    
    // Synchronize a component with its transform
    template<typename T>
    void synchronizeWithTransform(T* component);
    
    // Batch initialize multiple components
    template<typename... Components>
    void batchInitialize(Entity* entity) {
        BatchInitializer<Components...>::initialize(entity);
    }
    
    // Batch synchronize multiple components
    template<typename... Components>
    void batchSynchronize(Entity* entity) {
        BatchSynchronizer<Components...>::synchronize(entity);
    }
    
    // Helper to get component category name for debugging
    inline const char* getCategoryName(ComponentTraits::ComponentCategory category) {
        switch (category) {
            case ComponentTraits::ComponentCategory::Visual:     return "Visual";
            case ComponentTraits::ComponentCategory::Logic:      return "Logic";
            case ComponentTraits::ComponentCategory::Physics:    return "Physics";
            case ComponentTraits::ComponentCategory::Input:      return "Input";
            case ComponentTraits::ComponentCategory::Sound:      return "Sound";
            case ComponentTraits::ComponentCategory::Transform:  return "Transform";
            case ComponentTraits::ComponentCategory::Network:    return "Network";
            case ComponentTraits::ComponentCategory::Data:       return "Data";
            case ComponentTraits::ComponentCategory::System:     return "System";
            default:                                            return "None";
        }
    }
}

// Include the implementation
#include "ComponentUtils.inl"