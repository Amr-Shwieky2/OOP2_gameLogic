#pragma once
#include <SFML/System/Vector2.hpp>

class Entity;
class PlayerEntity;

/**
 * @brief Abstract base class for AI behavior strategies.
 *
 * This class defines the interface for AI strategies using the Strategy Pattern.
 * Each strategy defines its own behavior logic in the update() method.
 *
 * Example strategies might include: chasing the player, patrolling, or ambushing.
 */
class AIStrategy {
public:
    virtual ~AIStrategy() = default;

    /**
     * @brief Indicates whether the strategy requires access to the player entity.
     *
     * This is useful for strategies that do not depend on the player’s position (e.g., idle or patrol).
     * @return true if player information is needed, false otherwise.
     */
    virtual bool requiresPlayer() const { return true; }

    /**
     * @brief Updates the behavior of the given entity according to the strategy.
     *
     * @param entity The entity whose behavior is being updated.
     * @param dt Time elapsed since the last update (in seconds).
     * @param player Pointer to the player entity (may be null if not required).
     */
    virtual void update(Entity& entity, float dt, PlayerEntity* player) = 0;

    /**
     * @brief Returns the name of the strategy (for debugging/logging purposes).
     * @return A constant character string representing the strategy name.
     */
    virtual const char* getName() const = 0;

    /**
     * @brief Checks whether the player is visible to the entity.
     *
     * Basic implementation uses Euclidean distance; override for more advanced logic (e.g., raycasting).
     *
     * @param entityPos Position of the entity.
     * @param playerPos Position of the player.
     * @return true if player is within visible range, false otherwise.
     */
    virtual bool canSeePlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const;

protected:
    /**
     * @brief Computes the distance between the entity and the player.
     * @return Euclidean distance between the two positions.
     */
    float getDistanceToPlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const;

    /**
     * @brief Calculates a normalized direction vector pointing from the entity to the player.
     * @return A unit vector pointing toward the player (or zero vector if already at the same position).
     */
    sf::Vector2f getDirectionToPlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const;
};
