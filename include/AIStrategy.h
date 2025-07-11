// AIStrategy.h
#pragma once
#include <SFML/System/Vector2.hpp>

class Entity;
class PlayerEntity;

/**
 * Base class for AI strategies
 * Implements Strategy Pattern from course material
 * Enhanced to work with specialized AIComponent
 */
class AIStrategy {
public:
    virtual ~AIStrategy() = default;

    // Set the owner entity for this strategy
    void setOwner(Entity* owner) { m_owner = owner; }
    Entity* getOwner() const { return m_owner; }

    // Main update method used by AIComponent
    virtual void update(float dt) = 0;

    // Target detection events
    virtual void onTargetDetected(PlayerEntity* player) {}
    virtual void onTargetLost(const sf::Vector2f& lastKnownPosition) {}

    // Get strategy name for debugging
    virtual const char* getName() const = 0;

    // Can the AI see the player?
    virtual bool canSeePlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const;

protected:
    // Helper functions
    float getDistanceToPlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const;
    sf::Vector2f getDirectionToPlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const;
    
    // Owner entity - set by AIComponent
    Entity* m_owner = nullptr;
};