#pragma once
#include "AIStrategy.h"

/**
 * @brief AI strategy for patrolling back and forth horizontally.
 *
 * The PatrolStrategy makes the enemy move left and right along a fixed horizontal path.
 * It automatically reverses direction when reaching the patrol distance from the starting point.
 *
 * This behavior is independent of the player’s position.
 */
class PatrolStrategy : public AIStrategy {
public:
    /**
     * @brief Constructs the PatrolStrategy with a given distance and speed.
     *
     * @param patrolDistance The total distance (in pixels) the enemy will patrol in each direction.
     * @param speed The movement speed of the enemy in pixels per second.
     */
    PatrolStrategy(float patrolDistance = 200.0f, float speed = 50.0f);

    /**
     * @brief Updates the patrol movement of the enemy.
     *
     * Moves the enemy in the current direction. Reverses direction when the patrol boundary is reached.
     *
     * @param entity The enemy entity executing this strategy.
     * @param dt Time elapsed since last frame (in seconds).
     * @param player Unused in this strategy (can be null).
     */
    void update(Entity& entity, float dt, PlayerEntity* player) override;

    /**
     * @brief Returns the name of the strategy ("Patrol").
     */
    const char* getName() const override { return "Patrol"; }

    /**
     * @brief Indicates that this strategy does not depend on player presence.
     *
     * @return false Always returns false for patrol behavior.
     */
    bool requiresPlayer() const override { return false; }

private:
    float m_patrolDistance;  // Half-distance of the patrol path (from starting X).
    float m_speed;           // Movement speed in pixels per second.
    float m_startX = 0.0f;   // The initial X position to patrol around.
    int m_direction = 1;     // Current movement direction: +1 (right), -1 (left).
    bool m_initialized = false; // Flag to track if starting position has been set.
};
