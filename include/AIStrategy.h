// AIStrategy.h
#pragma once
#include <SFML/System/Vector2.hpp>

class Entity;
class PlayerEntity;

/**
 * Base class for AI strategies
 * Implements Strategy Pattern from course material
 */
class AIStrategy {
public:
    virtual ~AIStrategy() = default;

    virtual bool requiresPlayer() const { return true; }

    // Update AI behavior
    virtual void update(Entity& entity, float dt, PlayerEntity* player) = 0;

    // Get strategy name for debugging
    virtual const char* getName() const = 0;

    // Can the AI see the player?
    virtual bool canSeePlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const;

protected:
    // Helper functions
    float getDistanceToPlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const;
    sf::Vector2f getDirectionToPlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const;
};