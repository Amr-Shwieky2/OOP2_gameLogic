#include "Property/PropertyValue.h"
#include <sstream>
#include <iomanip>

bool PropertyValue::getBool() const {
    if (m_type == Type::Bool) {
        return std::get<bool>(m_value);
    }
    else if (m_type == Type::Int) {
        return std::get<int>(m_value) != 0;
    }
    else if (m_type == Type::Float) {
        return std::get<float>(m_value) != 0.0f;
    }
    else if (m_type == Type::String) {
        const std::string& str = std::get<std::string>(m_value);
        return !str.empty() && str != "0" && str != "false";
    }
    return false;
}

int PropertyValue::getInt() const {
    if (m_type == Type::Int) {
        return std::get<int>(m_value);
    }
    else if (m_type == Type::Bool) {
        return std::get<bool>(m_value) ? 1 : 0;
    }
    else if (m_type == Type::Float) {
        return static_cast<int>(std::get<float>(m_value));
    }
    else if (m_type == Type::String) {
        try {
            return std::stoi(std::get<std::string>(m_value));
        }
        catch (...) {
            return 0;
        }
    }
    else if (m_type == Type::Enum) {
        return std::get<int>(m_value);
    }
    return 0;
}

float PropertyValue::getFloat() const {
    if (m_type == Type::Float) {
        return std::get<float>(m_value);
    }
    else if (m_type == Type::Int) {
        return static_cast<float>(std::get<int>(m_value));
    }
    else if (m_type == Type::Bool) {
        return std::get<bool>(m_value) ? 1.0f : 0.0f;
    }
    else if (m_type == Type::String) {
        try {
            return std::stof(std::get<std::string>(m_value));
        }
        catch (...) {
            return 0.0f;
        }
    }
    return 0.0f;
}

std::string PropertyValue::getString() const {
    if (m_type == Type::String) {
        return std::get<std::string>(m_value);
    }
    
    return toString();
}

Vector2D PropertyValue::getVector2() const {
    if (m_type == Type::Vector2) {
        return std::get<Vector2D>(m_value);
    }
    return Vector2D();
}

sf::Vector2f PropertyValue::getSfVector2() const {
    Vector2D vec = getVector2();
    return sf::Vector2f(vec.x, vec.y);
}

sf::Color PropertyValue::getColor() const {
    if (m_type == Type::Color) {
        return std::get<sf::Color>(m_value);
    }
    return sf::Color::White;
}

int PropertyValue::getEnum() const {
    if (m_type == Type::Enum) {
        return std::get<int>(m_value);
    }
    return 0;
}

std::string PropertyValue::getEnumName() const {
    if (m_type == Type::Enum && m_enumNames) {
        int index = getEnum();
        if (index >= 0 && index < m_enumNames->size()) {
            return (*m_enumNames)[index];
        }
    }
    return {};
}

const std::vector<std::string>& PropertyValue::getEnumNames() const {
    static const std::vector<std::string> emptyNames;
    return m_enumNames ? *m_enumNames : emptyNames;
}

void PropertyValue::setValue(bool value) {
    m_value = value;
    m_type = Type::Bool;
}

void PropertyValue::setValue(int value) {
    if (m_type == Type::Enum && m_enumNames) {
        if (value >= 0 && value < m_enumNames->size()) {
            m_value = value;
        }
    }
    else {
        m_value = value;
        m_type = Type::Int;
    }
}

void PropertyValue::setValue(float value) {
    m_value = value;
    m_type = Type::Float;
}

void PropertyValue::setValue(const std::string& value) {
    m_value = value;
    m_type = Type::String;
}

void PropertyValue::setValue(const char* value) {
    m_value = std::string(value);
    m_type = Type::String;
}

void PropertyValue::setValue(const Vector2D& value) {
    m_value = value;
    m_type = Type::Vector2;
}

void PropertyValue::setValue(const sf::Vector2f& value) {
    setValue(Vector2D(value.x, value.y));
}

void PropertyValue::setValue(const sf::Color& value) {
    m_value = value;
    m_type = Type::Color;
}

void PropertyValue::setEnum(int value) {
    if (m_type == Type::Enum && m_enumNames) {
        if (value >= 0 && value < m_enumNames->size()) {
            m_value = value;
        }
    }
    else {
        m_value = value;
        m_type = Type::Enum;
    }
}

bool PropertyValue::operator==(const PropertyValue& other) const {
    if (m_type != other.m_type) {
        return false;
    }
    
    switch (m_type) {
    case Type::Bool:
        return std::get<bool>(m_value) == std::get<bool>(other.m_value);
    case Type::Int:
        return std::get<int>(m_value) == std::get<int>(other.m_value);
    case Type::Float:
        return std::abs(std::get<float>(m_value) - std::get<float>(other.m_value)) < 0.0001f;
    case Type::String:
        return std::get<std::string>(m_value) == std::get<std::string>(other.m_value);
    case Type::Vector2: {
        const auto& v1 = std::get<Vector2D>(m_value);
        const auto& v2 = std::get<Vector2D>(other.m_value);
        return v1 == v2;
    }
    case Type::Color: {
        const auto& c1 = std::get<sf::Color>(m_value);
        const auto& c2 = std::get<sf::Color>(other.m_value);
        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
    }
    case Type::Enum:
        return std::get<int>(m_value) == std::get<int>(other.m_value);
    default:
        return false;
    }
}

std::string PropertyValue::toString() const {
    std::ostringstream oss;
    
    switch (m_type) {
    case Type::Bool:
        oss << (std::get<bool>(m_value) ? "true" : "false");
        break;
    case Type::Int:
        oss << std::get<int>(m_value);
        break;
    case Type::Float:
        oss << std::fixed << std::setprecision(4) << std::get<float>(m_value);
        break;
    case Type::String:
        oss << std::get<std::string>(m_value);
        break;
    case Type::Vector2: {
        const auto& vec = std::get<Vector2D>(m_value);
        oss << vec.x << "," << vec.y;
        break;
    }
    case Type::Color: {
        const auto& color = std::get<sf::Color>(m_value);
        oss << static_cast<int>(color.r) << "," 
            << static_cast<int>(color.g) << ","
            << static_cast<int>(color.b) << ","
            << static_cast<int>(color.a);
        break;
    }
    case Type::Enum:
        if (m_enumNames && std::get<int>(m_value) >= 0 && std::get<int>(m_value) < m_enumNames->size()) {
            oss << (*m_enumNames)[std::get<int>(m_value)];
        }
        else {
            oss << std::get<int>(m_value);
        }
        break;
    default:
        oss << "Unknown";
        break;
    }
    
    return oss.str();
}

PropertyValue PropertyValue::fromString(const std::string& str, Type type) {
    PropertyValue result;
    
    try {
        switch (type) {
        case Type::Bool:
            result.setValue(str == "true" || str == "1" || str == "yes");
            break;
        case Type::Int:
            result.setValue(std::stoi(str));
            break;
        case Type::Float:
            result.setValue(std::stof(str));
            break;
        case Type::String:
            result.setValue(str);
            break;
        case Type::Vector2: {
            size_t commaPos = str.find(',');
            if (commaPos != std::string::npos) {
                float x = std::stof(str.substr(0, commaPos));
                float y = std::stof(str.substr(commaPos + 1));
                result.setValue(Vector2D(x, y));
            }
            break;
        }
        case Type::Color: {
            std::stringstream ss(str);
            std::string item;
            std::vector<int> components;
            
            while (std::getline(ss, item, ',')) {
                if (components.size() < 4) {
                    components.push_back(std::stoi(item));
                }
            }
            
            if (components.size() >= 3) {
                sf::Color color(
                    components[0],
                    components[1],
                    components[2],
                    components.size() > 3 ? components[3] : 255
                );
                result.setValue(color);
            }
            break;
        }
        default:
            result.setValue(str);
            break;
        }
    }
    catch (const std::exception&) {
        // In case of parsing errors, return a default value
        return PropertyValue();
    }
    
    return result;
}