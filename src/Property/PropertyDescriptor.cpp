#include "Property/PropertyDescriptor.h"
#include "Property/PropertyContainer.h"

PropertyDescriptor::PropertyDescriptor(
    const std::string& name,
    PropertyValue::Type type,
    GetterFunc getter,
    SetterFunc setter,
    Category category,
    uint32_t flags
) : m_name(name),
    m_type(type),
    m_category(category),
    m_flags(flags),
    m_getter(getter),
    m_setter(setter) {
}

PropertyValue PropertyDescriptor::getValue(PropertyContainer* container) const {
    if (!container || !m_getter) {
        return PropertyValue();
    }
    
    return m_getter(container);
}

void PropertyDescriptor::setValue(PropertyContainer* container, const PropertyValue& value) const {
    if (!container || !m_setter || hasFlag(Flags::ReadOnly)) {
        return;
    }
    
    // Get old value for change notification
    PropertyValue oldValue = getValue(container);
    
    // Only proceed if value actually changed
    if (oldValue != value) {
        // Apply the value using the setter
        m_setter(container, value);
        
        // Notify the container that a property changed
        container->onPropertyChanged(m_name, oldValue, value);
    }
}