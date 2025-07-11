#include "Component.h"
#include "Entity.h"
#include "Property/PropertyManager.h"
#include "Property/UndoManager.h"
#include "Property/PropertyEvents.h"
#include "EventSystem.h"
#include <iostream>
#include <string>

Component::Component() : m_owner(nullptr) {
    // Components are automatically registered when created
}

Component::~Component() {
    // Make sure to unregister from PropertyManager
    if (m_owner) {
        // Generate a container ID based on entity and component type
        std::string containerId = std::to_string(m_owner->getId()) + "_" + typeid(*this).name();
        PropertyManager::getInstance().unregisterContainer(containerId);
    }
}

void Component::setOwner(Entity* owner) {
    // If changing owners, unregister from old owner first
    if (m_owner && m_owner != owner) {
        std::string oldContainerId = std::to_string(m_owner->getId()) + "_" + typeid(*this).name();
        PropertyManager::getInstance().unregisterContainer(oldContainerId);
    }
    
    m_owner = owner;
    
    // Register with PropertyManager when owner is set
    if (m_owner) {
        std::string containerId = std::to_string(m_owner->getId()) + "_" + typeid(*this).name();
        PropertyManager::getInstance().registerContainer(this, containerId);
        
        // Publish event that container was registered
        EventSystem::getInstance().publish(
            PropertyRegisteredEvent(this, containerId)
        );
    }
}

Entity* Component::getOwner() const {
    return m_owner;
}

void Component::registerProperties() {
    // Base implementation - derived classes should override to register their properties
}

void Component::onPropertyChanged(const std::string& name, const PropertyValue& oldValue, const PropertyValue& newValue) {
    // Generate a container ID for event publishing
    std::string containerId;
    if (m_owner) {
        containerId = std::to_string(m_owner->getId()) + "_" + typeid(*this).name();
    }
    else {
        containerId = "unknown_" + std::to_string(reinterpret_cast<uintptr_t>(this));
    }
    
    // Record change for undo/redo
    UndoManager::getInstance().recordPropertyChange(
        this,
        name,
        oldValue,
        newValue,
        name + " changed"
    );
    
    // Publish property change event
    EventSystem::getInstance().publish(
        PropertyChangedEvent(this, containerId, name, oldValue, newValue)
    );
}