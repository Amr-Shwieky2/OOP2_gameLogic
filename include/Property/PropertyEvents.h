#pragma once
#include "../EventSystem.h"
#include "PropertyValue.h"
#include <string>

/**
 * PropertyChangedEvent - Fired when a property value changes
 */
class PropertyChangedEvent : public Event {
public:
    PropertyChangedEvent(
        void* container,
        const std::string& containerId,
        const std::string& propertyName,
        const PropertyValue& oldValue,
        const PropertyValue& newValue
    ) : container(container),
        containerId(containerId),
        propertyName(propertyName),
        oldValue(oldValue),
        newValue(newValue) {}

    const char* getName() const override { return "PropertyChanged"; }

    void* container;          // Pointer to the property container
    std::string containerId;  // ID of the container
    std::string propertyName; // Name of the property
    PropertyValue oldValue;   // Previous value
    PropertyValue newValue;   // New value
};

/**
 * PropertyRegisteredEvent - Fired when a property container is registered
 */
class PropertyRegisteredEvent : public Event {
public:
    PropertyRegisteredEvent(void* container, const std::string& containerId)
        : container(container), containerId(containerId) {}

    const char* getName() const override { return "PropertyRegistered"; }

    void* container;         // Pointer to the property container
    std::string containerId; // ID of the container
};

/**
 * PropertyUnregisteredEvent - Fired when a property container is unregistered
 */
class PropertyUnregisteredEvent : public Event {
public:
    PropertyUnregisteredEvent(void* container, const std::string& containerId)
        : container(container), containerId(containerId) {}

    const char* getName() const override { return "PropertyUnregistered"; }

    void* container;         // Pointer to the property container
    std::string containerId; // ID of the container
};

/**
 * MultiPropertyChangedEvent - Fired when multiple properties change at once
 * (e.g., during deserialization or undo/redo operations)
 */
class MultiPropertyChangedEvent : public Event {
public:
    MultiPropertyChangedEvent(void* container, const std::string& containerId)
        : container(container), containerId(containerId) {}

    const char* getName() const override { return "MultiPropertyChanged"; }

    void* container;         // Pointer to the property container
    std::string containerId; // ID of the container
};