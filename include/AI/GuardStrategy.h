#pragma once
#include "AIStrategy.h"

/**
 * @brief AI strategy for guarding a fixed position and attacking nearby players.
 *
 * The GuardStrategy makes an enemy stay near its original position (guard point).
 * It behaves as follows:
 * - If the player enters the attack range, the enemy stops and attacks.
 * - If the player enters the larger guard radius, the enemy moves toward the player.
 * - If the player leaves the guard radius, the enemy returns to its guard position.
 */
class GuardStrategy : public AIStrategy {
public:
    /**
     * @brief Constructs the GuardStrategy with specified behavior parameters.
     *
     * @param guardRadius The maximum distance from the guard position within which the enemy will chase the player.
     * @param attackRange The close range within which the enemy will stop and attack.
     */
    GuardStrategy(float guardRadius = 150.0f, float attackRange = 100.0f);

    /**
     * @brief Updates the enemy's behavior according to the guard logic.
     *
     * @param entity The enemy entity executing this strategy.
     * @param dt Time elapsed since the last frame (in seconds).
     * @param player Pointer to the player entity (can be null).
     */
    void update(Entity& entity, float dt, PlayerEntity* player) override;

    /**
     * @brief Returns the name of the strategy ("Guard").
     *
     * Useful for debugging and identifying the strategy type at runtime.
     */
    const char* getName() const override { return "Guard"; }

private:
    float m_guardRadius;           // The radius around the guard point in which the enemy chases the player.
    float m_attackRange;           // The range at which the enemy stops and attacks the player.
    sf::Vector2f m_guardPosition;  // The fixed position the enemy is assigned to guard.
    bool m_initialized = false;    // Whether the guard position has been initialized.
    float m_attackCooldown = 0.0f; // Cooldown timer to control attack frequency.
};
