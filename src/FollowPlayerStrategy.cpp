#include "FollowPlayerStrategy.h"
#include "Entity.h"
#include "PlayerEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

FollowPlayerStrategy::FollowPlayerStrategy(float speed, float detectionRange)
    : m_speed(speed)
    , m_detectionRange(detectionRange) {
    std::cout << "[FOLLOW] Strategy created - Speed: " << speed << " Range: " << detectionRange << std::endl;
}

void FollowPlayerStrategy::update(float) {
    if (!m_owner) return;
    
    auto* transform = m_owner->getComponent<Transform>();
    auto* physics = m_owner->getComponent<PhysicsComponent>();

    if (!transform || !physics) {
        return;
    }

    sf::Vector2f entityPos = transform->getPosition();
    
    // Debug output every 60 frames
    static int frameCount = 0;
    frameCount++;
    
    // If we have a target player, use direct tracking
    if (m_targetPlayer) {
        sf::Vector2f playerPos;
        if (auto* targetTransform = m_targetPlayer->getComponent<Transform>()) {
            playerPos = targetTransform->getPosition();
            
            float distance = getDistanceToPlayer(entityPos, playerPos);
            
            if (frameCount % 60 == 0) {
                std::cout << "[FOLLOW] Enemy " << m_owner->getId()
                    << " Distance to player: " << distance
                    << " (Range: " << m_detectionRange << ")" << std::endl;
            }
            
            // Only follow if player is within detection range
            if (distance <= m_detectionRange) {
                sf::Vector2f direction = getDirectionToPlayer(entityPos, playerPos);
                
                // Move towards player horizontally
                sf::Vector2f velocity = physics->getVelocity();
                physics->setVelocity(direction.x * (m_speed / PPM), velocity.y);
                
                if (frameCount % 60 == 0) {
                    std::cout << "[FOLLOW] Chasing player! Direction: " << direction.x
                        << " Velocity: " << (direction.x * m_speed) << std::endl;
                }
                
                // Remember that we're actively chasing
                m_isChasing = true;
                
                // Update last known position
                m_lastKnownPosition = playerPos;
                return;
            }
        }
    }
    
    // If we're not chasing a live target but have a last known position, move toward it
    if (!m_isChasing && m_lastKnownPosition != sf::Vector2f(0, 0)) {
        float dx = m_lastKnownPosition.x - entityPos.x;
        float distance = std::abs(dx);
        
        if (distance > 10.0f) { // Don't bother if we're close enough
            float direction = (dx > 0) ? 1.0f : -1.0f;
            sf::Vector2f velocity = physics->getVelocity();
            physics->setVelocity(direction * (m_speed * 0.7f / PPM), velocity.y); // Move a bit slower when searching
            
            if (frameCount % 60 == 0) {
                std::cout << "[FOLLOW] Moving to last known position" << std::endl;
            }
        }
        else {
            // We've reached the last known position, stop moving
            sf::Vector2f velocity = physics->getVelocity();
            physics->setVelocity(0, velocity.y);
            
            if (frameCount % 60 == 0) {
                std::cout << "[FOLLOW] Reached last known position, stopping" << std::endl;
            }
            
            // Clear the last known position
            m_lastKnownPosition = sf::Vector2f(0, 0);
        }
    }
    else {
        // No target, stop moving
        sf::Vector2f velocity = physics->getVelocity();
        physics->setVelocity(0, velocity.y);
    }
    
    // Not actively chasing anymore
    m_isChasing = false;
}

void FollowPlayerStrategy::onTargetDetected(PlayerEntity* player) {
    m_targetPlayer = player;
    m_isChasing = true;
    
    if (player && m_owner) {
        std::cout << "[FOLLOW] Entity " << m_owner->getId() << " detected player!" << std::endl;
    }
}

void FollowPlayerStrategy::onTargetLost(const sf::Vector2f& lastKnownPosition) {
    m_lastKnownPosition = lastKnownPosition;
    m_isChasing = false;
    
    if (m_owner) {
        std::cout << "[FOLLOW] Entity " << m_owner->getId() 
                  << " lost target at (" << lastKnownPosition.x 
                  << ", " << lastKnownPosition.y << ")" << std::endl;
    }
}