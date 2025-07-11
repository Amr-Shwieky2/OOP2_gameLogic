#pragma once

#include <SFML/System/Vector2.hpp>
#include <ostream>
#include <cmath>

/**
 * Vector2D - Custom 2D vector class with comprehensive operator overloading
 * 
 * This class provides a wrapper around sf::Vector2f with additional math operators
 * and functionality. It's meant to be used with Transform and other components
 * that need 2D vector operations.
 */
class Vector2D {
public:
    // Constructors
    Vector2D() : x(0.0f), y(0.0f) {}
    Vector2D(float x, float y) : x(x), y(y) {}
    Vector2D(const sf::Vector2f& vec) : x(vec.x), y(vec.y) {}
    
    // Conversion to sf::Vector2f
    operator sf::Vector2f() const { return sf::Vector2f(x, y); }
    sf::Vector2f toSfVector() const { return sf::Vector2f(x, y); }
    
    // Basic vector operations
    float length() const { return std::sqrt(x*x + y*y); }
    float lengthSquared() const { return x*x + y*y; }
    Vector2D normalized() const {
        float len = length();
        if (len < 0.0001f) return Vector2D();
        return Vector2D(x / len, y / len);
    }
    void normalize() {
        float len = length();
        if (len < 0.0001f) return;
        x /= len;
        y /= len;
    }
    
    // Dot and cross product
    float dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }
    float cross(const Vector2D& other) const {
        return x * other.y - y * other.x;
    }
    
    // Distance calculation
    float distance(const Vector2D& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        return std::sqrt(dx*dx + dy*dy);
    }
    float distanceSquared(const Vector2D& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        return dx*dx + dy*dy;
    }
    
    // Unary operators
    Vector2D operator-() const { return Vector2D(-x, -y); }
    Vector2D operator+() const { return *this; }
    
    // Assignment operators
    Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vector2D& operator-=(const Vector2D& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Vector2D& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Vector2D& operator/=(float scalar) {
        if (scalar != 0) {
            x /= scalar;
            y /= scalar;
        }
        return *this;
    }
    
    // Binary operators
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    Vector2D operator/(float scalar) const {
        if (scalar != 0) {
            return Vector2D(x / scalar, y / scalar);
        }
        return *this;
    }
    
    // Comparison operators
    bool operator==(const Vector2D& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Vector2D& other) const {
        return !(*this == other);
    }
    
    // Vector comparison (useful for sorting vectors in containers)
    bool operator<(const Vector2D& other) const {
        if (x == other.x) return y < other.y;
        return x < other.x;
    }
    bool operator>(const Vector2D& other) const {
        if (x == other.x) return y > other.y;
        return x > other.x;
    }
    bool operator<=(const Vector2D& other) const {
        return *this < other || *this == other;
    }
    bool operator>=(const Vector2D& other) const {
        return *this > other || *this == other;
    }
    
    // Subscript operator for array-like access (0 for x, 1 for y)
    float& operator[](int index) {
        return index == 0 ? x : y;
    }
    const float& operator[](int index) const {
        return index == 0 ? x : y;
    }
    
    // Public data
    float x;
    float y;
};

// Global scalar multiplication
inline Vector2D operator*(float scalar, const Vector2D& vec) {
    return vec * scalar;
}

// Stream insertion operator for debugging
inline std::ostream& operator<<(std::ostream& os, const Vector2D& vec) {
    os << "Vector2D(" << vec.x << ", " << vec.y << ")";
    return os;
}

// Stream extraction operator for input
inline std::istream& operator>>(std::istream& is, Vector2D& vec) {
    // Format expected: (x,y)
    char c;
    is >> c; // Skip (
    is >> vec.x;
    is >> c; // Skip ,
    is >> vec.y;
    is >> c; // Skip )
    return is;
}

// Common vector operations
inline float dot(const Vector2D& v1, const Vector2D& v2) {
    return v1.dot(v2);
}

inline float cross(const Vector2D& v1, const Vector2D& v2) {
    return v1.cross(v2);
}

inline float distance(const Vector2D& v1, const Vector2D& v2) {
    return v1.distance(v2);
}

inline float distanceSquared(const Vector2D& v1, const Vector2D& v2) {
    return v1.distanceSquared(v2);
}

inline Vector2D normalize(const Vector2D& vec) {
    return vec.normalized();
}

// Vector2D constants
namespace Vector2DConstants {
    const Vector2D Zero(0.0f, 0.0f);
    const Vector2D One(1.0f, 1.0f);
    const Vector2D UnitX(1.0f, 0.0f);
    const Vector2D UnitY(0.0f, 1.0f);
    const Vector2D NegativeUnitX(-1.0f, 0.0f);
    const Vector2D NegativeUnitY(0.0f, -1.0f);
}