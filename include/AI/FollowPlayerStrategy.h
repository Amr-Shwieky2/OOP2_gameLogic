#pragma once
#include "AIStrategy.h"

/**
 * @brief AI strategy for following the player when within a specified range.
 *
 * This strategy makes the entity move horizontally toward the player if the player is
 * within a certain detection range. It simulates a simple chasing behavior and ignores vertical movement.
 *
 * It inherits from AIStrategy and overrides the update behavior.
 */
class FollowPlayerStrategy : public AIStrategy {
public:
    /**
     * @brief Constructs the strategy with a given speed and detection range.
     *
     * @param speed Movement speed of the entity in pixels per second (default: 100).
     * @param detectionRange Maximum distance at which the entity will start following the player (default: 300).
     */
    FollowPlayerStrategy(float speed = 100.0f, float detectionRange = 300.0f);

    /**
     * @brief Updates the entity’s behavior to follow the player.
     *
     * If the player is within the detection range, the entity moves toward them horizontally.
     * If the player is out of range or null, the entity stops moving.
     *
     * @param entity The enemy entity executing the strategy.
     * @param dt Time elapsed since last frame (in seconds).
     * @param player Pointer to the player entity.
     */
    void update(Entity& entity, float dt, PlayerEntity* player) override;

    /**
     * @brief Returns the name of the strategy ("FollowPlayer").
     *
     * Useful for debugging/logging purposes.
     */
    const char* getName() const override { return "FollowPlayer"; }

private:
    float m_speed;           // Movement speed of the enemy (in pixels per second)
    float m_detectionRange;  // Maximum distance for detecting and following the player
};
