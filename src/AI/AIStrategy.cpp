#include "AIStrategy.h"
#include <cmath>

//-------------------------------------------------------------------------------------
bool AIStrategy::canSeePlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const {
    float distance = getDistanceToPlayer(entityPos, playerPos);
    return distance < 500.0f;
}
//-------------------------------------------------------------------------------------
float AIStrategy::getDistanceToPlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const {
    sf::Vector2f diff = playerPos - entityPos;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}
//-------------------------------------------------------------------------------------
sf::Vector2f AIStrategy::getDirectionToPlayer(const sf::Vector2f& entityPos, const sf::Vector2f& playerPos) const {
    sf::Vector2f direction = playerPos - entityPos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0) {
        direction /= length; 
    }
    return direction;
}
//-------------------------------------------------------------------------------------