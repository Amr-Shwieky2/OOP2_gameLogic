#pragma once
#include "Component.h"
#include <SFML/Graphics/Rect.hpp>

/**
 * CollisionComponent - Defines collision properties
 * Used by collision system to identify entity types
 */
class CollisionComponent : public Component {
public:
    enum class CollisionType {
        Player,
        Enemy,
        Projectile,
        Collectible,
        Obstacle,
        Ground,
        Hazard
    };

    CollisionComponent(CollisionType type);

    CollisionType getType() const { return m_type; }

    // Collision bounds (can be different from render bounds)
    void setBounds(const sf::FloatRect& bounds) { m_bounds = bounds; }
    sf::FloatRect getBounds() const { return m_bounds; }

    // Layer system for filtering collisions
    void setLayer(uint16_t layer) { m_layer = layer; }
    uint16_t getLayer() const { return m_layer; }

    void setMask(uint16_t mask) { m_mask = mask; }
    uint16_t getMask() const { return m_mask; }

private:
    CollisionType m_type;
    sf::FloatRect m_bounds;
    uint16_t m_layer = 1;  // What layer this entity is on
    uint16_t m_mask = 0xFFFF;  // What layers it collides with
};