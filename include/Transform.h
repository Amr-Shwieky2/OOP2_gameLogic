#pragma once
#include "Component.h"
#include "Vector2D.h"
#include <SFML/System/Vector2.hpp>

class Transform : public Component {
public:
    Transform();
    Transform(const Vector2D& position);
    Transform(const sf::Vector2f& position);
    Transform(float x, float y);

    // Position methods
    void setPosition(const Vector2D& pos);
    void setPosition(const sf::Vector2f& pos);
    void setPosition(float x, float y);
    Vector2D getPosition() const;
    sf::Vector2f getSfPosition() const; // For SFML compatibility

    void move(const Vector2D& delta);
    void move(const sf::Vector2f& delta);
    void move(float dx, float dy);

    // Rotation (in degrees)
    void setRotation(float angle);
    float getRotation() const;
    void rotate(float delta);

    // Scale
    void setScale(const Vector2D& scale);
    void setScale(const sf::Vector2f& scale);
    void setScale(float x, float y);
    Vector2D getScale() const;
    void scale(float factorX, float factorY);
    
    // Vector arithmetic operators
    Transform& operator+=(const Vector2D& offset);
    Transform& operator-=(const Vector2D& offset);
    Transform& operator*=(float scalar); // Scale transform
    
    // Comparison operators for sorting/ordering transforms
    bool operator==(const Transform& other) const;
    bool operator!=(const Transform& other) const;
    bool operator<(const Transform& other) const;
    bool operator>(const Transform& other) const;
    
    // Function call operator for position modification
    Vector2D operator()(const Vector2D& point) const;
    
    // Output stream operator for debugging (declared as friend)
    friend std::ostream& operator<<(std::ostream& os, const Transform& transform);

private:
    Vector2D m_position;
    float m_rotation{ 0.f }; // Degrees
    Vector2D m_scale{ 1.f, 1.f };
};

// Free-standing operators for Transform
Transform operator+(const Transform& transform, const Vector2D& offset);
Transform operator-(const Transform& transform, const Vector2D& offset);
Transform operator*(const Transform& transform, float scalar);
Transform operator*(float scalar, const Transform& transform);

// Stream insertion operator
std::ostream& operator<<(std::ostream& os, const Transform& transform);