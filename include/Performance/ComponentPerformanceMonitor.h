#pragma once
#include "Performance/Profiler.h"
#include "Entity.h"
#include "Component.h"
#include <typeinfo>
#include <typeindex>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace Performance {

/**
 * @brief Performance monitor for the component system
 * 
 * This class integrates with the existing component system to monitor
 * the performance and memory usage of components.
 */
class ComponentPerformanceMonitor {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the ComponentPerformanceMonitor
     */
    static ComponentPerformanceMonitor& getInstance();
    
    /**
     * @brief Set performance budget for a component type
     * 
     * @tparam T Component type
     * @param operation Operation name (e.g., "update", "render")
     * @param budgetMs Performance budget in milliseconds
     */
    template<typename T>
    void setComponentBudget(const std::string& operation, double budgetMs) {
        std::string name = getComponentFullName<T>(operation);
        ProfilingManager::getInstance().setPerformanceBudget(name, budgetMs);
    }
    
    /**
     * @brief Profile a component operation
     * 
     * @tparam T Component type
     * @param component Component pointer
     * @param operation Operation name
     * @return Unique pointer to a ScopedProfiler
     */
    template<typename T>
    static std::unique_ptr<ScopedProfiler> profileComponent(T* component, const std::string& operation) {
        if (!component) {
            return nullptr;
        }
        
        std::string name = getComponentFullName<T>(operation);
        return std::make_unique<ScopedProfiler>(name, "Component");
    }
    
    /**
     * @brief Profile an entity operation
     * 
     * @param entity Entity pointer
     * @param operation Operation name
     * @return Unique pointer to a ScopedProfiler
     */
    static std::unique_ptr<ScopedProfiler> profileEntity(Entity* entity, const std::string& operation) {
        if (!entity) {
            return nullptr;
        }
        
        std::string name = "Entity_" + std::to_string(entity->getId()) + "::" + operation;
        return std::make_unique<ScopedProfiler>(name, "Entity");
    }
    
    /**
     * @brief Register a component for performance monitoring
     * 
     * @tparam T Component type
     * @param defaultBudgetMs Default performance budget in milliseconds
     */
    template<typename T>
    void registerComponent(double defaultBudgetMs = 0.0) {
        std::string typeName = typeid(T).name();
        m_componentNames[std::type_index(typeid(T))] = typeName;
        
        if (defaultBudgetMs > 0.0) {
            setComponentBudget<T>("update", defaultBudgetMs);
        }
    }
    
    /**
     * @brief Get the performance impact rating for a component type
     * 
     * @tparam T Component type
     * @return Performance impact rating (1-10, 10 being highest impact)
     */
    template<typename T>
    int getPerformanceImpact() const {
        // Using the existing trait from ComponentTraits
        return ComponentTraits::getPerformanceImpact<T>();
    }
    
    /**
     * @brief Set a callback for when a component exceeds its budget
     * 
     * @param callback Function to call when budget is exceeded
     */
    void setBudgetExceededCallback(std::function<void(const std::string&, double, double)> callback) {
        m_budgetExceededCallback = callback;
        ProfilingManager::getInstance().setBudgetExceededCallback(callback);
    }
    
    /**
     * @brief Generate a performance report for all components
     * @return Report string
     */
    std::string generateComponentReport() const;
    
private:
    // Private constructor for singleton
    ComponentPerformanceMonitor();
    ~ComponentPerformanceMonitor() = default;
    
    // Get the full name for a component operation
    template<typename T>
    static std::string getComponentFullName(const std::string& operation) {
        std::string typeName = typeid(T).name();
        return "Component::" + typeName + "::" + operation;
    }
    
    // Map from type to name
    std::unordered_map<std::type_index, std::string> m_componentNames;
    
    // Budget exceeded callback
    std::function<void(const std::string&, double, double)> m_budgetExceededCallback;
};

// Convenience macro for component operation profiling
#define PROFILE_COMPONENT_OP(component, operation) \
    auto __componentProfiler = Performance::ComponentPerformanceMonitor::profileComponent(component, operation)

// Convenience macro for entity operation profiling
#define PROFILE_ENTITY_OP(entity, operation) \
    auto __entityProfiler = Performance::ComponentPerformanceMonitor::profileEntity(entity, operation)

} // namespace Performance