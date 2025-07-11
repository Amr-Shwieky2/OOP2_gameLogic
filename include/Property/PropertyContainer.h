#pragma once
#include "PropertyDescriptor.h"
#include "PropertyValue.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

/**
 * PropertyContainer - Interface for objects that expose properties
 * 
 * This class is the base for any object that wants to expose properties
 * for the property system. Components can inherit from this interface
 * to provide reflection-like capabilities.
 */
class PropertyContainer {
public:
    virtual ~PropertyContainer() = default;
    
    // Register properties - called once per class during initialization
    virtual void registerProperties() = 0;
    
    // Get all property descriptors for this container
    const std::vector<std::shared_ptr<PropertyDescriptor>>& getPropertyDescriptors() const;
    
    // Get a specific property descriptor by name
    std::shared_ptr<PropertyDescriptor> getPropertyDescriptor(const std::string& name) const;
    
    // Get/set property values by name
    PropertyValue getPropertyValue(const std::string& name) const;
    void setPropertyValue(const std::string& name, const PropertyValue& value);
    
    // Check if a property exists
    bool hasProperty(const std::string& name) const;
    
    // Events for property changes
    virtual void onPropertyChanged(const std::string& name, const PropertyValue& oldValue, const PropertyValue& newValue);
    
protected:
    // Helper to register property with getter/setter
    template<typename T, typename GetterReturnType>
    void registerProperty(
        const std::string& name,
        PropertyValue::Type type,
        GetterReturnType(T::*getter)() const,
        void(T::*setter)(GetterReturnType),
        PropertyDescriptor::Category category = PropertyDescriptor::Category::Basic,
        uint32_t flags = PropertyDescriptor::Flags::Serializable
    ) {
        auto descriptor = std::make_shared<PropertyDescriptor>(
            name,
            type,
            [getter, this](PropertyContainer* container) -> PropertyValue {
                T* typedThis = static_cast<T*>(container);
                return PropertyValue((typedThis->*getter)());
            },
            [setter, this](PropertyContainer* container, const PropertyValue& value) {
                T* typedThis = static_cast<T*>(container);
                if constexpr (std::is_same_v<GetterReturnType, bool>) {
                    (typedThis->*setter)(value.getBool());
                }
                else if constexpr (std::is_same_v<GetterReturnType, int>) {
                    (typedThis->*setter)(value.getInt());
                }
                else if constexpr (std::is_same_v<GetterReturnType, float>) {
                    (typedThis->*setter)(value.getFloat());
                }
                else if constexpr (std::is_same_v<GetterReturnType, std::string>) {
                    (typedThis->*setter)(value.getString());
                }
                else if constexpr (std::is_same_v<GetterReturnType, Vector2D>) {
                    (typedThis->*setter)(value.getVector2());
                }
                else if constexpr (std::is_same_v<GetterReturnType, sf::Vector2f>) {
                    (typedThis->*setter)(value.getSfVector2());
                }
                else if constexpr (std::is_same_v<GetterReturnType, sf::Color>) {
                    (typedThis->*setter)(value.getColor());
                }
                else {
                    // Fallback for other types
                    (typedThis->*setter)(GetterReturnType());
                }
            },
            category,
            flags
        );
        
        m_propertyDescriptors.push_back(descriptor);
        m_propertyDescriptorMap[name] = descriptor;
    }
    
    // Register a read-only property with just a getter
    template<typename T, typename GetterReturnType>
    void registerReadOnlyProperty(
        const std::string& name,
        PropertyValue::Type type,
        GetterReturnType(T::*getter)() const,
        PropertyDescriptor::Category category = PropertyDescriptor::Category::Basic,
        uint32_t flags = PropertyDescriptor::Flags::Serializable | PropertyDescriptor::Flags::ReadOnly
    ) {
        auto descriptor = std::make_shared<PropertyDescriptor>(
            name,
            type,
            [getter, this](PropertyContainer* container) -> PropertyValue {
                T* typedThis = static_cast<T*>(container);
                return PropertyValue((typedThis->*getter)());
            },
            [](PropertyContainer*, const PropertyValue&) {
                // No-op setter for read-only property
            },
            category,
            flags | PropertyDescriptor::Flags::ReadOnly
        );
        
        m_propertyDescriptors.push_back(descriptor);
        m_propertyDescriptorMap[name] = descriptor;
    }
    
    // Register an enum property
    template<typename T, typename EnumType>
    void registerEnumProperty(
        const std::string& name,
        EnumType(T::*getter)() const,
        void(T::*setter)(EnumType),
        const std::vector<std::string>& enumNames,
        PropertyDescriptor::Category category = PropertyDescriptor::Category::Basic,
        uint32_t flags = PropertyDescriptor::Flags::Serializable
    ) {
        auto descriptor = std::make_shared<PropertyDescriptor>(
            name,
            PropertyValue::Type::Enum,
            [getter, enumNames, this](PropertyContainer* container) -> PropertyValue {
                T* typedThis = static_cast<T*>(container);
                int value = static_cast<int>((typedThis->*getter)());
                return PropertyValue(value, enumNames);
            },
            [setter, this](PropertyContainer* container, const PropertyValue& value) {
                T* typedThis = static_cast<T*>(container);
                (typedThis->*setter)(static_cast<EnumType>(value.getEnum()));
            },
            category,
            flags
        );
        
        descriptor->setEnumNames(enumNames);
        m_propertyDescriptors.push_back(descriptor);
        m_propertyDescriptorMap[name] = descriptor;
    }
    
    // Configure property display attributes
    void configureProperty(
        const std::string& name,
        const std::string& displayName,
        const std::string& description = ""
    );
    
    // Configure property range (for numeric types)
    void setPropertyRange(const std::string& name, double min, double max);
    
    // Configure property default value
    void setPropertyDefaultValue(const std::string& name, const PropertyValue& defaultValue);
    
private:
    // Store property descriptors for this container
    std::vector<std::shared_ptr<PropertyDescriptor>> m_propertyDescriptors;
    std::unordered_map<std::string, std::shared_ptr<PropertyDescriptor>> m_propertyDescriptorMap;
    
    // Flag to check if properties have been registered
    mutable bool m_propertiesRegistered = false;
    
    // Helper to ensure properties are registered
    void ensurePropertiesRegistered() const;
};