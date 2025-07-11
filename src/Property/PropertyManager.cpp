#include "Property/PropertyManager.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

PropertyManager& PropertyManager::getInstance() {
    static PropertyManager instance;
    return instance;
}

void PropertyManager::registerContainer(PropertyContainer* container, const std::string& id) {
    if (!container || id.empty()) {
        return;
    }
    
    m_containers[id] = container;
    m_containerIds[container] = id;
    
    // Ensure properties are registered
    container->getPropertyDescriptors();
}

void PropertyManager::unregisterContainer(const std::string& id) {
    auto it = m_containers.find(id);
    if (it != m_containers.end()) {
        m_containerIds.erase(it->second);
        m_containers.erase(it);
    }
}

void PropertyManager::unregisterContainer(PropertyContainer* container) {
    if (!container) {
        return;
    }
    
    auto it = m_containerIds.find(container);
    if (it != m_containerIds.end()) {
        m_containers.erase(it->second);
        m_containerIds.erase(it);
    }
}

PropertyContainer* PropertyManager::getContainer(const std::string& id) const {
    auto it = m_containers.find(id);
    return (it != m_containers.end()) ? it->second : nullptr;
}

std::vector<PropertyContainer*> PropertyManager::getAllContainers() const {
    std::vector<PropertyContainer*> result;
    result.reserve(m_containers.size());
    
    for (const auto& [id, container] : m_containers) {
        result.push_back(container);
    }
    
    return result;
}

void PropertyManager::serializeAll(const std::string& filename) {
    json j;
    
    // Serialize each container
    for (const auto& [id, container] : m_containers) {
        json containerJson;
        
        for (const auto& descriptor : container->getPropertyDescriptors()) {
            // Skip non-serializable properties
            if (!descriptor->hasFlag(PropertyDescriptor::Flags::Serializable)) {
                continue;
            }
            
            // Get the property value
            PropertyValue value = descriptor->getValue(container);
            containerJson[descriptor->getName()] = value.toString();
        }
        
        j[id] = containerJson;
    }
    
    // Write to file
    try {
        std::ofstream file(filename);
        file << j.dump(2); // Pretty print with 2-space indent
    }
    catch (const std::exception& e) {
        std::cerr << "Error serializing properties: " << e.what() << std::endl;
    }
}

bool PropertyManager::deserializeAll(const std::string& filename) {
    try {
        std::ifstream file(filename);
        json j;
        file >> j;
        
        // Deserialize each container
        for (const auto& [id, containerJson] : j.items()) {
            auto* container = getContainer(id);
            if (!container) {
                std::cerr << "Container not found: " << id << std::endl;
                continue;
            }
            
            // Deserialize each property
            for (const auto& [propertyName, valueStr] : containerJson.items()) {
                auto descriptor = container->getPropertyDescriptor(propertyName);
                if (!descriptor) {
                    std::cerr << "Property not found: " << propertyName << std::endl;
                    continue;
                }
                
                // Skip read-only properties
                if (descriptor->hasFlag(PropertyDescriptor::Flags::ReadOnly)) {
                    continue;
                }
                
                // Convert string to PropertyValue
                PropertyValue value = PropertyValue::fromString(
                    valueStr.get<std::string>(), 
                    descriptor->getType()
                );
                
                // Set the property value
                descriptor->setValue(container, value);
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error deserializing properties: " << e.what() << std::endl;
        return false;
    }
}

std::string PropertyManager::serializeContainer(const std::string& containerId) {
    json j;
    
    auto* container = getContainer(containerId);
    if (!container) {
        return "{}";
    }
    
    for (const auto& descriptor : container->getPropertyDescriptors()) {
        // Skip non-serializable properties
        if (!descriptor->hasFlag(PropertyDescriptor::Flags::Serializable)) {
            continue;
        }
        
        // Get the property value
        PropertyValue value = descriptor->getValue(container);
        j[descriptor->getName()] = value.toString();
    }
    
    return j.dump();
}

bool PropertyManager::deserializeContainer(const std::string& containerId, const std::string& serializedData) {
    try {
        auto* container = getContainer(containerId);
        if (!container) {
            return false;
        }
        
        json j = json::parse(serializedData);
        
        // Deserialize each property
        for (const auto& [propertyName, valueStr] : j.items()) {
            auto descriptor = container->getPropertyDescriptor(propertyName);
            if (!descriptor) {
                std::cerr << "Property not found: " << propertyName << std::endl;
                continue;
            }
            
            // Skip read-only properties
            if (descriptor->hasFlag(PropertyDescriptor::Flags::ReadOnly)) {
                continue;
            }
            
            // Convert string to PropertyValue
            PropertyValue value = PropertyValue::fromString(
                valueStr.get<std::string>(), 
                descriptor->getType()
            );
            
            // Set the property value
            descriptor->setValue(container, value);
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error deserializing container: " << e.what() << std::endl;
        return false;
    }
}