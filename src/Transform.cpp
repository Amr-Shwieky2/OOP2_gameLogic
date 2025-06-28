#include "Transform.h"

Transform::Transform() : m_position(0.f, 0.f), m_rotation(0.f), m_scale(1.f, 1.f) {}

Transform::Transform(const sf::Vector2f& position) : m_position(position), m_rotation(0.f), m_scale(1.f, 1.f) {}

// Position
void Transform::setPosition(const sf::Vector2f& pos) { m_position = pos; }
void Transform::setPosition(float x, float y) { m_position = { x, y }; }
sf::Vector2f Transform::getPosition() const { return m_position; }
void Transform::move(const sf::Vector2f& delta) { m_position += delta; }
void Transform::move(float dx, float dy) { m_position.x += dx; m_position.y += dy; }

// Rotation
void Transform::setRotation(float angle) { m_rotation = angle; }
float Transform::getRotation() const { return m_rotation; }
void Transform::rotate(float delta) { m_rotation += delta; }

// Scale
void Transform::setScale(const sf::Vector2f& scale) { m_scale = scale; }
void Transform::setScale(float x, float y) { m_scale = { x, y }; }
sf::Vector2f Transform::getScale() const { return m_scale; }
void Transform::scale(float factorX, float factorY) { m_scale.x *= factorX; m_scale.y *= factorY; }