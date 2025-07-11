#pragma once
#include "Entity.h"
#include "ComponentTraits.h"
#include "ForwardDeclarations.h"
#include <type_traits>
#include <tuple>
#include <iostream>

/**
 * DependencyResolver - Automatically resolves component dependencies
 * 
 * This utility uses template meta-programming to analyze component dependencies
 * at compile-time and create missing components in the correct order when needed.
 * It uses the ComponentTraits system to determine dependencies.
 */
class DependencyResolver {
public:
    // Main interface - ensure an entity has a component and all its dependencies
    template<typename T, typename... Args>
    static T* ensureComponent(Entity& entity, Args&&... args) {
        // First check if the component already exists
        T* component = entity.getComponent<T>();
        if (component) {
            return component; // Component already exists
        }

        // Resolve dependencies first
        resolveDependencies<T>(entity);
        
        // Now create the component
        return entity.addComponent<T>(std::forward<Args>(args)...);
    }
    
    // Batch version for ensuring multiple components
    template<typename... Components>
    static void ensureComponents(Entity& entity) {
        (ensureComponent<Components>(entity), ...); // C++17 fold expression
    }

private:
    // Recursively resolve dependencies for a component type
    template<typename T>
    static void resolveDependencies(Entity& entity) {
        // Get dependencies tuple from traits
        using Dependencies = typename ComponentTraits::Dependencies<T>::type;
        
        // Expand the dependencies tuple and ensure each dependency
        expandDependenciesAndResolve<T, Dependencies>(entity);
    }
    
    // Helper to expand a tuple of dependencies
    template<typename T, typename TupleDeps>
    static void expandDependenciesAndResolve(Entity& entity) {
        // Dispatch to tuple expansion helper
        expandTuple<TupleDeps>(entity, std::make_index_sequence<std::tuple_size_v<TupleDeps>>{});
    }
    
    // Helper to expand a tuple using index sequence
    template<typename Tuple, std::size_t... Is>
    static void expandTuple(Entity& entity, std::index_sequence<Is...>) {
        // Create each dependency in sequence
        (resolveSingleDependency<std::tuple_element_t<Is, Tuple>>(entity), ...);
    }
    
    // Resolve a single dependency
    template<typename Dep>
    static void resolveSingleDependency(Entity& entity) {
        // If the dependency doesn't exist, resolve its dependencies and then create it
        if (!entity.hasComponent<Dep>()) {
            // Recursively resolve this dependency's dependencies
            resolveDependencies<Dep>(entity);
            
            // Create the dependency itself
            std::cout << "Auto-creating dependency: " << typeid(Dep).name() << std::endl;
            entity.addComponent<Dep>();
        }
    }
};

// Convenience functions
namespace EntityUtils {
    // Ensure an entity has a component and its dependencies
    template<typename T, typename... Args>
    T* ensureComponent(Entity& entity, Args&&... args) {
        return DependencyResolver::ensureComponent<T>(entity, std::forward<Args>(args)...);
    }
    
    // Ensure an entity has multiple components and their dependencies
    template<typename... Components>
    void ensureComponents(Entity& entity) {
        DependencyResolver::ensureComponents<Components...>(entity);
    }
}