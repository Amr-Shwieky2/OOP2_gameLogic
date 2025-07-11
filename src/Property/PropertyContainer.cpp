#include "Property/PropertyContainer.h"
#include <iostream>

const std::vector<std::shared_ptr<PropertyDescriptor>>& PropertyContainer::getPropertyDescriptors() const {
    ensurePropertiesRegistered();
    return m_propertyDescriptors;
}

std::shared_ptr<PropertyDescriptor> PropertyContainer::getPropertyDescriptor(const std::string& name) const {
    ensurePropertiesRegistered();
    auto it = m_propertyDescriptorMap.find(name);
    return (it != m_propertyDescriptorMap.end()) ? it->second : nullptr;
}

PropertyValue PropertyContainer::getPropertyValue(const std::string& name) const {
    ensurePropertiesRegistered();
    auto descriptor = getPropertyDescriptor(name);
    if (descriptor) {
        return descriptor->getValue(const_cast<PropertyContainer*>(this));
    }
    return PropertyValue();
}

void PropertyContainer::setPropertyValue(const std::string& name, const PropertyValue& value) {
    ensurePropertiesRegistered();
    auto descriptor = getPropertyDescriptor(name);
    if (descriptor) {
        descriptor->setValue(this, value);
    }
}

bool PropertyContainer::hasProperty(const std::string& name) const {
    ensurePropertiesRegistered();
    return m_propertyDescriptorMap.find(name) != m_propertyDescriptorMap.end();
}

void PropertyContainer::onPropertyChanged(const std::string& name, const PropertyValue& oldValue, const PropertyValue& newValue) {
    // Base implementation does nothing - derived classes can override to respond to changes
    // or publish events
}

void PropertyContainer::configureProperty(
    const std::string& name,
    const std::string& displayName,
    const std::string& description
) {
    auto descriptor = getPropertyDescriptor(name);
    if (descriptor) {
        descriptor->setDisplayName(displayName);
        descriptor->setDescription(description);
    }
}

void PropertyContainer::setPropertyRange(const std::string& name, double min, double max) {
    auto descriptor = getPropertyDescriptor(name);
    if (descriptor) {
        descriptor->setRange(min, max);
    }
}

void PropertyContainer::setPropertyDefaultValue(const std::string& name, const PropertyValue& defaultValue) {
    auto descriptor = getPropertyDescriptor(name);
    if (descriptor) {
        descriptor->setDefaultValue(defaultValue);
    }
}

void PropertyContainer::ensurePropertiesRegistered() const {
    if (!m_propertiesRegistered) {
        const_cast<PropertyContainer*>(this)->registerProperties();
        m_propertiesRegistered = true;
    }
}