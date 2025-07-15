#pragma once

#include "Entity.h"
#include <Box2D/Box2D.h>
#include "ResourceManager.h"

/**
 * @brief ProjectileEntity - Represents a bullet or projectile fired by the player or enemy.
 *
 * This class supports both gravity-affected and straight projectiles.
 * It configures physics, rendering, and collision behavior based on parameters like gravity and origin.
 */
class ProjectileEntity : public Entity {
public:
    /**
     * @brief Constructor to create a projectile entity.
     *
     * @param id            Unique identifier for the projectile.
     * @param world         Reference to the Box2D world.
     * @param x             Initial X position.
     * @param y             Initial Y position.
     * @param direction     Direction vector of the projectile.
     * @param textures      Reference to the texture manager.
     * @param fromPlayer    Whether the projectile was fired by the player (true) or enemy (false).
     * @param withGravity   Whether gravity should affect the projectile.
     */
    ProjectileEntity(IdType id, b2World& world, float x, float y,
        sf::Vector2f direction, TextureManager& textures,
        bool fromPlayer = true, bool withGravity = false);

    /**
     * @brief Check whether this projectile was fired by the player.
     * @return True if from player, false if from enemy.
     */
    bool isFromPlayer() const { return m_fromPlayer; }

    /**
     * @brief Update method to handle movement, lifetime, and off-screen detection.
     * @param dt Delta time since last frame.
     */
    void update(float dt) override;

private:
    /**
     * @brief Internal method to add and configure all required components.
     *
     * @param world         Box2D physics world.
     * @param x             Initial X position.
     * @param y             Initial Y position.
     * @param direction     Movement direction of the projectile.
     * @param textures      Texture manager for loading sprite.
     * @param withGravity   If true, simulate gravity on the projectile.
     */
    void setupComponents(b2World& world, float x, float y,
        sf::Vector2f direction, TextureManager& textures, bool withGravity);

private:
    bool m_fromPlayer;     ///< True if projectile is fired by the player.
    bool m_withGravity;    ///< True if gravity affects this projectile.
    float m_lifetime = 3.0f;   ///< Max lifetime of the projectile in seconds.
    float m_stopTime = 0.0f;   ///< Time duration the projectile has been stationary.

    // Gravity simulation parameters
    sf::Vector2f m_velocity;  ///< Velocity used when gravity is applied manually.
    static constexpr float GRAVITY = 9.8f; ///< Constant used for gravity simulation.
};
