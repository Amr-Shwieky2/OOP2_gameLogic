#pragma once
#include "Component.h"
#include <SFML/Graphics/Rect.hpp>

/**
 * @brief Component that defines collision-related data for an entity.
 *
 * This component stores information needed by the collision system, including:
 * - The type of the entity (used for collision logic, e.g., Player vs Enemy)
 * - Collision bounds (can differ from the render bounds)
 * - Layer and mask system for collision filtering (bitwise-based)
 *
 * It does not implement collision logic itself — only holds data.
 */
class CollisionComponent : public Component {
public:
    /**
     * @brief Enum representing the type of the entity for collision categorization.
     */
    enum class CollisionType {
        Player,
        Enemy,
        Projectile,
        Collectible,
        Obstacle,
        Ground,
        Hazard
    };

    /**
     * @brief Constructs the component with a specified collision type.
     * @param type The category of this entity for collision handling.
     */
    CollisionComponent(CollisionType type) : m_type(type) {}

    /**
     * @brief Returns the collision type of the entity.
     */
    CollisionType getType() const { return m_type; }

    // === Collision bounds ===

    /**
     * @brief Sets the local bounding box used for collision detection.
     */
    void setBounds(const sf::FloatRect& bounds) { m_bounds = bounds; }

    /**
     * @brief Returns the current collision bounds.
     */
    sf::FloatRect getBounds() const { return m_bounds; }

    // === Layer and Mask System ===

    /**
     * @brief Sets the collision layer this entity belongs to (e.g., 1 = Player layer).
     */
    void setLayer(uint16_t layer) { m_layer = layer; }

    /**
     * @brief Returns the collision layer of the entity.
     */
    uint16_t getLayer() const { return m_layer; }

    /**
     * @brief Sets the collision mask, determining which layers this entity can collide with.
     */
    void setMask(uint16_t mask) { m_mask = mask; }

    /**
     * @brief Returns the collision mask.
     */
    uint16_t getMask() const { return m_mask; }

private:
    CollisionType m_type;           ///< Type of the entity (used for collision logic).
    sf::FloatRect m_bounds;         ///< Bounding box for collision (in local coordinates).
    uint16_t m_layer = 1;           ///< Bitmask layer this entity is assigned to.
    uint16_t m_mask = 0xFFFF;       ///< Bitmask of layers this entity can collide with.
};
