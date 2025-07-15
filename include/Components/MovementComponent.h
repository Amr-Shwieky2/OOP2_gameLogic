#pragma once
#include "Component.h"
#include <SFML/System/Vector2.hpp>

/**
 * @class MovementComponent
 * @brief Handles various movement behaviors for an entity, such as linear, circular, and following movement.
 *
 * This component is responsible for updating an entity's position over time according to a selected movement type.
 * It supports direct transform manipulation or integration with PhysicsComponent when available.
 */
class MovementComponent : public Component {
public:
    /**
     * @enum MovementType
     * @brief Defines available movement patterns.
     */
    enum class MovementType {
        Static,     ///< No movement
        Linear,     ///< Constant velocity in a direction
        Following,  ///< Follows a target position
        Circular,   ///< Circular orbit around a point
        Sine,       ///< Oscillates in sine-wave pattern
        Custom      ///< Reserved for user-defined behavior
    };

    /**
     * @brief Constructor to initialize with a specific movement type.
     * @param type The type of movement behavior to use.
     */
    explicit MovementComponent(MovementType type = MovementType::Static);

    /**
     * @brief Updates the entity's position based on current movement type and delta time.
     * @param dt Time since last frame (delta time).
     */
    void update(float dt) override;

    // --- Configuration methods ---

    /**
     * @brief Sets the linear movement speed.
     * @param speed Speed in units per second.
     */
    void setSpeed(float speed) { m_speed = speed; }

    /**
     * @brief Sets the direction for linear movement.
     * @param dir Unit vector representing movement direction.
     */
    void setDirection(const sf::Vector2f& dir) { m_direction = dir; }

    /**
     * @brief Sets the target position used in "Following" movement mode.
     * @param target The destination point to move toward.
     */
    void setTarget(const sf::Vector2f& target) { m_target = target; }

    /**
     * @brief Configures circular motion parameters and switches to Circular mode.
     * @param center Center point of the circle.
     * @param radius Radius of the circular path.
     * @param speed Angular speed in radians per second.
     */
    void setCircularMotion(const sf::Vector2f& center, float radius, float speed);

private:
    MovementType m_type;             ///< Type of movement behavior
    float m_speed = 100.0f;          ///< Movement speed (linear or angular)
    sf::Vector2f m_direction{ 1.0f, 0.0f }; ///< Direction vector for linear motion
    sf::Vector2f m_target;           ///< Target position (for following)

    // --- Circular and sine motion support ---
    sf::Vector2f m_circleCenter;     ///< Center point for circular/sine motion
    float m_circleRadius = 0.0f;     ///< Radius for circular/sine motion
    float m_angle = 0.0f;            ///< Current angle for circular motion
};
