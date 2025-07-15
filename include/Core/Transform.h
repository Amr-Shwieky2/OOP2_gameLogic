#pragma once
#include "Component.h"
#include <SFML/System/Vector2.hpp>

/**
 * @class Transform
 * @brief Component that handles position, rotation, and scale of an entity.
 *
 * Provides basic spatial manipulation operations similar to Unity's Transform.
 * Used by rendering, physics, and AI systems for positioning and movement.
 */
class Transform : public Component {
public:
    /**
     * @brief Default constructor. Initializes position to (0,0), rotation to 0, and scale to (1,1).
     */
    Transform();

    /**
     * @brief Constructor with initial position.
     * @param position Initial position of the entity.
     */
    Transform(const sf::Vector2f& position);

    // -------------------- Position --------------------

    /**
     * @brief Set the position of the entity.
     */
    void setPosition(const sf::Vector2f& pos);
    void setPosition(float x, float y);

    /**
     * @brief Get the current position of the entity.
     */
    sf::Vector2f getPosition() const;

    /**
     * @brief Move the entity by a delta offset.
     */
    void move(const sf::Vector2f& delta);
    void move(float dx, float dy);

    // -------------------- Rotation --------------------

    /**
     * @brief Set the rotation angle (in degrees).
     */
    void setRotation(float angle);

    /**
     * @brief Get the current rotation (in degrees).
     */
    float getRotation() const;

    /**
     * @brief Increment rotation by a delta angle.
     */
    void rotate(float delta);

    // -------------------- Scale --------------------

    /**
     * @brief Set the scale of the entity.
     */
    void setScale(const sf::Vector2f& scale);
    void setScale(float x, float y);

    /**
     * @brief Get the current scale.
     */
    sf::Vector2f getScale() const;

    /**
     * @brief Scale the current scale by factors along each axis.
     */
    void scale(float factorX, float factorY);

private:
    sf::Vector2f m_position{ 0.f, 0.f };   ///< Position in world space
    float m_rotation{ 0.f };              ///< Rotation in degrees (clockwise)
    sf::Vector2f m_scale{ 1.f, 1.f };     ///< Scale along x and y axes
};
