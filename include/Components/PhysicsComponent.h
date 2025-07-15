#pragma once
#include "Component.h"
#include <Box2D/Box2D.h>
#include <SFML/System/Vector2.hpp>

/**
 * @class PhysicsComponent
 * @brief Handles physical simulation for entities using Box2D.
 *
 * Provides position, velocity, and force-based movement for entities.
 * Replaces direct manipulation of positions in gameplay code.
 */
class PhysicsComponent : public Component {
public:
    /**
     * @brief Constructor - creates a Box2D body in the world.
     * @param world Reference to the Box2D world.
     * @param type Type of the body (dynamic, static, kinematic).
     */
    PhysicsComponent(b2World& world, b2BodyType type = b2_dynamicBody);

    /**
     * @brief Destructor - safely destroys the Box2D body.
     */
    ~PhysicsComponent();

    // --- Component interface ---
    /**
     * @brief Syncs the entity's Transform with its physics body.
     * @param dt Delta time (not used here).
     */
    void update(float dt) override;

    /**
     * @brief Called when the component is destroyed, cleans up Box2D body.
     */
    void onDestroy() override;

    // --- Physics controls ---

    /**
     * @brief Sets the physics body's position.
     * @param x X position in pixels.
     * @param y Y position in pixels.
     */
    void setPosition(float x, float y);

    /**
     * @brief Gets the position of the physics body.
     * @return Position in SFML (pixels).
     */
    sf::Vector2f getPosition() const;

    /**
     * @brief Sets the linear velocity of the body.
     * @param x X velocity.
     * @param y Y velocity.
     */
    void setVelocity(float x, float y);

    /**
     * @brief Gets the current linear velocity.
     * @return Velocity as vector.
     */
    sf::Vector2f getVelocity() const;

    /**
     * @brief Applies a continuous force to the center of mass.
     * @param x Force on X axis.
     * @param y Force on Y axis.
     */
    void applyForce(float x, float y);

    /**
     * @brief Applies an instant impulse to the center of mass.
     * @param x Impulse on X axis.
     * @param y Impulse on Y axis.
     */
    void applyImpulse(float x, float y);

    // --- Direct access to body ---

    /**
     * @brief Access the raw Box2D body.
     * @return Pointer to b2Body.
     */
    b2Body* getBody() { return m_body; }

    /**
     * @brief Const access to the raw Box2D body.
     * @return Const pointer to b2Body.
     */
    const b2Body* getBody() const { return m_body; }

    // --- Shape configuration ---

    /**
     * @brief Creates a circular fixture shape on the body.
     * @param radius Circle radius in pixels.
     */
    void createCircleShape(float radius);

    /**
     * @brief Creates a rectangular fixture shape on the body.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param density Physical density (default 1.0f).
     * @param friction Surface friction (default 0.3f).
     * @param restitution Bounciness (default 0.1f).
     */
    void createBoxShape(float width, float height,
        float density = 1.0f,
        float friction = 0.3f,
        float restitution = 0.1f);

private:
    b2Body* m_body = nullptr;   ///< Pointer to the Box2D physics body.
    b2World& m_world;           ///< Reference to the physics world (not owned).
};
