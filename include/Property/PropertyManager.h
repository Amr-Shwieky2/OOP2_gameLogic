#pragma once
#include "PropertyContainer.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

/**
 * PropertyManager - Central registry for property containers
 * 
 * This singleton manages all property containers in the system,
 * facilitating querying and bulk operations on properties.
 */
class PropertyManager {
public:
    // Singleton instance
    static PropertyManager& getInstance();
    
    // Register a property container
    void registerContainer(PropertyContainer* container, const std::string& id);
    
    // Unregister a property container
    void unregisterContainer(const std::string& id);
    void unregisterContainer(PropertyContainer* container);
    
    // Get a container by ID
    PropertyContainer* getContainer(const std::string& id) const;
    
    // Get all registered containers
    std::vector<PropertyContainer*> getAllContainers() const;
    
    // Get containers by type
    template<typename T>
    std::vector<T*> getContainersByType() const {
        std::vector<T*> result;
        for (const auto& [id, container] : m_containers) {
            if (T* typedContainer = dynamic_cast<T*>(container)) {
                result.push_back(typedContainer);
            }
        }
        return result;
    }
    
    // Serialization
    void serializeAll(const std::string& filename);
    bool deserializeAll(const std::string& filename);
    
    // Serialization for a specific container
    std::string serializeContainer(const std::string& containerId);
    bool deserializeContainer(const std::string& containerId, const std::string& serializedData);
    
private:
    PropertyManager() = default;
    ~PropertyManager() = default;
    
    // Map of container ID to container pointer
    std::unordered_map<std::string, PropertyContainer*> m_containers;
    
    // Reverse map from container pointer to ID
    std::unordered_map<PropertyContainer*, std::string> m_containerIds;
};