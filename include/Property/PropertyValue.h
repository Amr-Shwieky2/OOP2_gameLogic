#pragma once
#include <string>
#include <variant>
#include <memory>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include "../Vector2D.h"

/**
 * PropertyValue - Type-safe wrapper for property values with reflection-like capabilities
 * 
 * This class can store various property types and provides type-safe access
 * with proper conversions between compatible types.
 */
class PropertyValue {
public:
    // Supported property types
    enum class Type {
        Bool,
        Int,
        Float,
        String,
        Vector2,
        Color,
        Enum,
        Object,   // Reference to another object/entity
        Array     // Array of values
    };

    // Constructors for different property types
    PropertyValue() : m_value(0), m_type(Type::Int) {}
    PropertyValue(bool value) : m_value(value), m_type(Type::Bool) {}
    PropertyValue(int value) : m_value(value), m_type(Type::Int) {}
    PropertyValue(float value) : m_value(value), m_type(Type::Float) {}
    PropertyValue(const std::string& value) : m_value(value), m_type(Type::String) {}
    PropertyValue(const char* value) : m_value(std::string(value)), m_type(Type::String) {}
    PropertyValue(const sf::Vector2f& value) : m_value(Vector2D(value.x, value.y)), m_type(Type::Vector2) {}
    PropertyValue(const Vector2D& value) : m_value(value), m_type(Type::Vector2) {}
    PropertyValue(const sf::Color& value) : m_value(value), m_type(Type::Color) {}
    
    // Enum value constructor with name mapping
    PropertyValue(int enumValue, const std::vector<std::string>& enumNames) 
        : m_value(enumValue), m_type(Type::Enum), m_enumNames(std::make_shared<std::vector<std::string>>(enumNames)) {}

    // Get the property value type
    Type getType() const { return m_type; }

    // Type checking methods
    bool isBool() const { return m_type == Type::Bool; }
    bool isInt() const { return m_type == Type::Int; }
    bool isFloat() const { return m_type == Type::Float; }
    bool isString() const { return m_type == Type::String; }
    bool isVector2() const { return m_type == Type::Vector2; }
    bool isColor() const { return m_type == Type::Color; }
    bool isEnum() const { return m_type == Type::Enum; }
    bool isNumber() const { return isInt() || isFloat(); }

    // Type conversion methods
    bool getBool() const;
    int getInt() const;
    float getFloat() const;
    std::string getString() const;
    Vector2D getVector2() const;
    sf::Vector2f getSfVector2() const;
    sf::Color getColor() const;
    int getEnum() const;
    std::string getEnumName() const;
    const std::vector<std::string>& getEnumNames() const;

    // Value setter methods
    void setValue(bool value);
    void setValue(int value);
    void setValue(float value);
    void setValue(const std::string& value);
    void setValue(const char* value);
    void setValue(const Vector2D& value);
    void setValue(const sf::Vector2f& value);
    void setValue(const sf::Color& value);
    void setEnum(int value);

    // Operators
    bool operator==(const PropertyValue& other) const;
    bool operator!=(const PropertyValue& other) const { return !(*this == other); }
    
    // Output as string (for debugging and serialization)
    std::string toString() const;
    
    // Create from string representation (for deserialization)
    static PropertyValue fromString(const std::string& str, Type type);
    
private:
    // Value storage using std::variant for type safety
    using ValueVariant = std::variant<bool, int, float, std::string, Vector2D, sf::Color>;
    ValueVariant m_value;
    Type m_type;
    
    // For enum types, store the name mapping
    std::shared_ptr<std::vector<std::string>> m_enumNames;
};