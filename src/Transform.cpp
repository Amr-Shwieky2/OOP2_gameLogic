#include "Transform.h"
#include <cmath>
#include <ostream>

Transform::Transform()
    : m_position(0.0f, 0.0f), m_rotation(0.0f), m_scale(1.0f, 1.0f) {
}

Transform::Transform(const Vector2D& position)
    : m_position(position), m_rotation(0.0f), m_scale(1.0f, 1.0f) {
}

Transform::Transform(const sf::Vector2f& position)
    : m_position(position.x, position.y), m_rotation(0.0f), m_scale(1.0f, 1.0f) {
}

Transform::Transform(float x, float y)
    : m_position(x, y), m_rotation(0.0f), m_scale(1.0f, 1.0f) {
}

void Transform::setPosition(const Vector2D& pos) {
    m_position = pos;
}

void Transform::setPosition(const sf::Vector2f& pos) {
    m_position.x = pos.x;
    m_position.y = pos.y;
}

void Transform::setPosition(float x, float y) {
    m_position.x = x;
    m_position.y = y;
}

Vector2D Transform::getPosition() const {
    return m_position;
}

sf::Vector2f Transform::getSfPosition() const {
    return sf::Vector2f(m_position.x, m_position.y);
}

void Transform::move(const Vector2D& delta) {
    m_position += delta;
}

void Transform::move(const sf::Vector2f& delta) {
    m_position.x += delta.x;
    m_position.y += delta.y;
}

void Transform::move(float dx, float dy) {
    m_position.x += dx;
    m_position.y += dy;
}

void Transform::setRotation(float angle) {
    m_rotation = angle;
}

float Transform::getRotation() const {
    return m_rotation;
}

void Transform::rotate(float delta) {
    m_rotation += delta;
}

void Transform::setScale(const Vector2D& scale) {
    m_scale = scale;
}

void Transform::setScale(const sf::Vector2f& scale) {
    m_scale.x = scale.x;
    m_scale.y = scale.y;
}

void Transform::setScale(float x, float y) {
    m_scale.x = x;
    m_scale.y = y;
}

Vector2D Transform::getScale() const {
    return m_scale;
}

void Transform::scale(float factorX, float factorY) {
    m_scale.x *= factorX;
    m_scale.y *= factorY;
}

Transform& Transform::operator+=(const Vector2D& offset) {
    move(offset);
    return *this;
}

Transform& Transform::operator-=(const Vector2D& offset) {
    move(-offset);
    return *this;
}

Transform& Transform::operator*=(float scalar) {
    m_scale.x *= scalar;
    m_scale.y *= scalar;
    return *this;
}

bool Transform::operator==(const Transform& other) const {
    return m_position == other.m_position && 
           std::abs(m_rotation - other.m_rotation) < 0.001f && 
           m_scale == other.m_scale;
}

bool Transform::operator!=(const Transform& other) const {
    return !(*this == other);
}

bool Transform::operator<(const Transform& other) const {
    // Compare based on position, then rotation, then scale
    if (m_position.y < other.m_position.y) return true;
    if (m_position.y > other.m_position.y) return false;
    if (m_position.x < other.m_position.x) return true;
    if (m_position.x > other.m_position.x) return false;
    return m_rotation < other.m_rotation;
}

bool Transform::operator>(const Transform& other) const {
    return other < *this;
}

Vector2D Transform::operator()(const Vector2D& point) const {
    // Apply transform to point
    Vector2D result = point;
    
    // Apply scale
    result.x *= m_scale.x;
    result.y *= m_scale.y;
    
    // Apply rotation
    float rad = m_rotation * 3.14159f / 180.0f;
    float cos_r = std::cos(rad);
    float sin_r = std::sin(rad);
    
    float x = result.x * cos_r - result.y * sin_r;
    float y = result.x * sin_r + result.y * cos_r;
    
    result.x = x;
    result.y = y;
    
    // Apply translation
    result += m_position;
    
    return result;
}

// Free-standing operators
Transform operator+(const Transform& transform, const Vector2D& offset) {
    Transform result = transform;
    result += offset;
    return result;
}

Transform operator-(const Transform& transform, const Vector2D& offset) {
    Transform result = transform;
    result -= offset;
    return result;
}

Transform operator*(const Transform& transform, float scalar) {
    Transform result = transform;
    result *= scalar;
    return result;
}

Transform operator*(float scalar, const Transform& transform) {
    return transform * scalar;
}

// Stream insertion operator implementation
std::ostream& operator<<(std::ostream& os, const Transform& transform) {
    os << "Transform(pos=(" << transform.m_position.x << ", " << transform.m_position.y
       << "), rot=" << transform.m_rotation
       << ", scale=(" << transform.m_scale.x << ", " << transform.m_scale.y << "))";
    return os;
}

// Register properties for the Transform component
void Transform::registerProperties() {
    // Call base implementation
    Component::registerProperties();
    
    // Register position properties
    registerProperty<Transform, Vector2D>(
        "position",
        PropertyValue::Type::Vector2,
        &Transform::getPosition,
        &Transform::setPosition,
        PropertyDescriptor::Category::Transform
    );
    
    // Register individual position components for easier editing
    registerProperty<Transform, float>(
        "positionX",
        PropertyValue::Type::Float,
        [](const Transform* t) -> float { return t->getPosition().x; },
        [](Transform* t, float value) { t->setPosition(value, t->getPosition().y); },
        PropertyDescriptor::Category::Transform
    );
    
    registerProperty<Transform, float>(
        "positionY",
        PropertyValue::Type::Float,
        [](const Transform* t) -> float { return t->getPosition().y; },
        [](Transform* t, float value) { t->setPosition(t->getPosition().x, value); },
        PropertyDescriptor::Category::Transform
    );
    
    // Register rotation property
    registerProperty<Transform, float>(
        "rotation",
        PropertyValue::Type::Float,
        &Transform::getRotation,
        &Transform::setRotation,
        PropertyDescriptor::Category::Transform
    );
    
    // Register scale properties
    registerProperty<Transform, Vector2D>(
        "scale",
        PropertyValue::Type::Vector2,
        &Transform::getScale,
        &Transform::setScale,
        PropertyDescriptor::Category::Transform
    );
    
    registerProperty<Transform, float>(
        "scaleX",
        PropertyValue::Type::Float,
        [](const Transform* t) -> float { return t->getScale().x; },
        [](Transform* t, float value) { t->setScale(value, t->getScale().y); },
        PropertyDescriptor::Category::Transform
    );
    
    registerProperty<Transform, float>(
        "scaleY",
        PropertyValue::Type::Float,
        [](const Transform* t) -> float { return t->getScale().y; },
        [](Transform* t, float value) { t->setScale(t->getScale().x, value); },
        PropertyDescriptor::Category::Transform
    );
    
    // Configure display names and descriptions
    configureProperty("position", "Position", "The position of the entity in world space");
    configureProperty("positionX", "X Position", "The X coordinate of the entity's position");
    configureProperty("positionY", "Y Position", "The Y coordinate of the entity's position");
    configureProperty("rotation", "Rotation", "The rotation angle in degrees");
    configureProperty("scale", "Scale", "The scale factor applied to the entity");
    configureProperty("scaleX", "X Scale", "The horizontal scale factor");
    configureProperty("scaleY", "Y Scale", "The vertical scale factor");
    
    // Set property ranges
    setPropertyRange("rotation", 0.0, 360.0);
    setPropertyRange("scaleX", 0.01, 10.0);
    setPropertyRange("scaleY", 0.01, 10.0);
}