#pragma once
#include "PropertyValue.h"
#include <string>
#include <functional>
#include <limits>

class PropertyContainer;

/**
 * PropertyDescriptor - Describes property metadata
 * 
 * This class holds all metadata about a property including name, type,
 * getter/setter functions, and validation/display information.
 */
class PropertyDescriptor {
public:
    using GetterFunc = std::function<PropertyValue(PropertyContainer*)>;
    using SetterFunc = std::function<void(PropertyContainer*, const PropertyValue&)>;
    
    // Property display categories for UI organization
    enum class Category {
        Basic,        // Basic properties shown by default
        Transform,    // Position, rotation, scale
        Visual,       // Visual appearance properties
        Physics,      // Physics simulation properties
        Behavior,     // AI and behavior properties
        Advanced,     // Advanced properties typically hidden
        Debug         // Debug-only properties
    };
    
    // Property flags for additional metadata
    enum Flags : uint32_t {
        None          = 0,
        ReadOnly      = 1 << 0,  // Property cannot be modified
        Hidden        = 1 << 1,  // Property is not displayed in UI
        Animatable    = 1 << 2,  // Property can be animated
        Serializable  = 1 << 3,  // Property should be serialized
        Required      = 1 << 4,  // Property must have a value
        NoUndo        = 1 << 5,  // Changes don't go on undo stack
        RestartNeeded = 1 << 6,  // Changes require restart to take effect
        RangeRestricted = 1 << 7 // Property has min/max values
    };
    
    PropertyDescriptor(
        const std::string& name,
        PropertyValue::Type type,
        GetterFunc getter,
        SetterFunc setter,
        Category category = Category::Basic,
        uint32_t flags = Flags::Serializable
    );
    
    // Basic property info
    const std::string& getName() const { return m_name; }
    PropertyValue::Type getType() const { return m_type; }
    Category getCategory() const { return m_category; }
    uint32_t getFlags() const { return m_flags; }
    
    // Display information
    const std::string& getDisplayName() const { return m_displayName.empty() ? m_name : m_displayName; }
    const std::string& getDescription() const { return m_description; }
    void setDisplayName(const std::string& displayName) { m_displayName = displayName; }
    void setDescription(const std::string& description) { m_description = description; }
    
    // Flag operations
    bool hasFlag(Flags flag) const { return (m_flags & flag) != 0; }
    void setFlag(Flags flag, bool value = true) {
        if (value) m_flags |= flag;
        else m_flags &= ~flag;
    }
    
    // Value operations
    PropertyValue getValue(PropertyContainer* container) const;
    void setValue(PropertyContainer* container, const PropertyValue& value) const;
    
    // Range constraints for numeric properties
    void setRange(double min, double max) {
        m_minValue = min;
        m_maxValue = max;
        setFlag(RangeRestricted);
    }
    
    double getMinValue() const { return m_minValue; }
    double getMaxValue() const { return m_maxValue; }
    bool hasRange() const { return hasFlag(RangeRestricted); }
    
    // Enum support
    void setEnumNames(const std::vector<std::string>& names) { m_enumNames = names; }
    const std::vector<std::string>& getEnumNames() const { return m_enumNames; }
    
    // Default value
    void setDefaultValue(const PropertyValue& defaultValue) { m_defaultValue = defaultValue; }
    const PropertyValue& getDefaultValue() const { return m_defaultValue; }
    
private:
    std::string m_name;                // Property identifier
    std::string m_displayName;         // User-friendly name for UI
    std::string m_description;         // Property description/tooltip
    PropertyValue::Type m_type;        // Property data type
    Category m_category;               // UI category
    uint32_t m_flags;                  // Property flags
    
    GetterFunc m_getter;               // Function to get value
    SetterFunc m_setter;               // Function to set value
    
    double m_minValue = std::numeric_limits<double>::lowest();
    double m_maxValue = std::numeric_limits<double>::max();
    PropertyValue m_defaultValue;      // Default value
    std::vector<std::string> m_enumNames; // Names for enum values
};