#pragma once
#include <SFML/Graphics.hpp>

class FalconEnemyEntity;

/**
 * @brief FalconWeaponSystem - Responsible for handling shooting logic of the Falcon enemy.
 *
 * This class encapsulates the behavior for when and how the Falcon enemy
 * fires projectiles at the player. It keeps track of timing, cooldowns,
 * and the readiness state, and interfaces with GameSession to spawn projectiles.
 */
class FalconWeaponSystem {
public:
    /**
     * @brief Constructor that takes a reference to the owning Falcon enemy entity.
     * @param falcon Reference to the FalconEnemyEntity that owns this weapon system.
     */
    explicit FalconWeaponSystem(FalconEnemyEntity& falcon);

    /**
     * @brief Update method called every frame to handle shooting cooldown and fire bullets.
     * @param dt Delta time since the last frame (in seconds).
     */
    void update(float dt);

    /**
     * @brief Resets internal state such as timers and shooting flags.
     */
    void reset();

    /**
     * @brief Sets whether the Falcon is in a state where it is allowed to shoot.
     * This can be controlled by AI or visibility.
     * @param ready True to enable shooting, false to disable.
     */
    void setReadyToShoot(bool ready);

    /**
     * @brief Returns whether the Falcon is currently allowed to shoot.
     * @return True if ready to shoot, false otherwise.
     */
    bool isReadyToShoot() const { return m_readyToShoot; }

private:
    FalconEnemyEntity& m_falcon;     ///< Reference to the Falcon enemy that owns this system.
    float m_shootTimer = 0.0f;       ///< Accumulates time for cooldown tracking.
    float m_shootCooldown = 1.0f;    ///< Minimum time between shots (in seconds).
    bool m_readyToShoot = false;     ///< Whether the Falcon is currently allowed to shoot.
    bool m_hasEnteredScreen = false; ///< Whether the Falcon has entered the visible screen area.

    /**
     * @brief Internal method to instantiate and spawn a projectile from the Falcon.
     */
    void shootProjectile();
};
