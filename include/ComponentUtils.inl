#pragma once
#include "Entity.h"
#include <iostream>
#include <tuple>
#include <type_traits>

namespace ComponentUtils {

    // Implementation of AreDependenciesSatisfied
    template<typename T, typename FirstDep, typename... RestDeps>
    bool AreDependenciesSatisfied<T, FirstDep, RestDeps...>::check(Entity* entity) {
        if (!entity) return false;
        
        // Check if the entity has the first dependency
        if (!entity->hasComponent<FirstDep>()) {
            std::cerr << "Component dependency missing: " 
                      << typeid(T).name() << " requires " 
                      << typeid(FirstDep).name() << std::endl;
            return false;
        }
        
        // Recursively check remaining dependencies
        return AreDependenciesSatisfied<T, RestDeps...>::check(entity);
    }
    
    // Implementation of BatchInitializer
    template<typename FirstComponent, typename... RestComponents>
    void BatchInitializer<FirstComponent, RestComponents...>::initialize(Entity* entity) {
        if (!entity) return;
        
        auto* component = entity->getComponent<FirstComponent>();
        if (component) {
            // Check for initialization method using SFINAE
            if constexpr (HasInitializeMethod<FirstComponent>::value) {
                component->initialize();
            }
        }
        
        // Recursively initialize the rest
        BatchInitializer<RestComponents...>::initialize(entity);
    }
    
    // Implementation of BatchSynchronizer
    template<typename FirstComponent, typename... RestComponents>
    void BatchSynchronizer<FirstComponent, RestComponents...>::synchronize(Entity* entity) {
        if (!entity) return;
        
        auto* component = entity->getComponent<FirstComponent>();
        if (component) {
            // Check for synchronizeWithTransform method using SFINAE
            if constexpr (HasSynchronizeMethod<FirstComponent>::value) {
                component->synchronizeWithTransform();
            }
        }
        
        // Recursively synchronize the rest
        BatchSynchronizer<RestComponents...>::synchronize(entity);
    }
    
    // Implementation of canAddComponent
    template<typename T>
    bool canAddComponent(Entity* entity) {
        if (!entity) return false;
        
        // Check if entity already has this component
        if (entity->hasComponent<T>()) {
            std::cerr << "Entity already has component: " << typeid(T).name() << std::endl;
            return false;
        }
        
        // Check compatibility with existing components
        bool compatible = true;
        
        // This requires access to entity's components which we don't have
        // Ideally, Entity would provide a way to iterate over components
        // For now, we'll leave this as a placeholder
        
        // Extract dependencies from traits
        using Dependencies = typename ComponentTraits::Dependencies<T>::type;
        
        // Check if dependencies are satisfied using tuple expansion
        if constexpr (std::tuple_size_v<Dependencies> > 0) {
            using DependencyChecker = DependencyCheckerImpl<T, Dependencies>;
            compatible = DependencyChecker::check(entity);
        }
        
        return compatible;
    }
    
    // Implementation of initializeComponent
    template<typename T>
    bool initializeComponent(T* component, Entity* entity) {
        if (!component || !entity) return false;
        
        // Validate dependencies
        if (!validateDependencies<T>(component, entity)) {
            return false;
        }
        
        // Initialize if the component has an initialize method
        if constexpr (HasInitializeMethod<T>::value) {
            return component->initialize();
        }
        
        return true;
    }
    
    // Implementation of validateDependencies
    template<typename T>
    bool validateDependencies(T* component, Entity* entity) {
        if (!component || !entity) return false;
        
        // Check if component has a validateDependencies method
        if constexpr (HasValidateDependenciesMethod<T>::value) {
            return component->validateDependencies();
        }
        
        // Otherwise, check dependencies from traits
        using Dependencies = typename ComponentTraits::Dependencies<T>::type;
        
        if constexpr (std::tuple_size_v<Dependencies> > 0) {
            return checkDependencies<T, Dependencies>(entity);
        }
        
        return true;
    }
    
    // Implementation of synchronizeWithTransform
    template<typename T>
    void synchronizeWithTransform(T* component) {
        if (!component) return;
        
        // Check if component has a synchronizeWithTransform method
        if constexpr (HasSynchronizeMethod<T>::value) {
            component->synchronizeWithTransform();
        }
    }
    
    // Helper template to check dependencies using tuple
    template<typename T, typename TupleDeps>
    struct DependencyCheckerImpl;
    
    template<typename T, typename... Deps>
    struct DependencyCheckerImpl<T, std::tuple<Deps...>> {
        static bool check(Entity* entity) {
            return AreDependenciesSatisfied<T, Deps...>::check(entity);
        }
    };
    
    // Helper to check dependencies from a tuple type
    template<typename T, typename TupleDeps>
    bool checkDependencies(Entity* entity) {
        return DependencyCheckerImpl<T, TupleDeps>::check(entity);
    }
}