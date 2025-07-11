#include "AIComponent.h"
#include "AIStrategy.h"
#include "Entity.h"
#include "PlayerEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include <iostream>
#include <cmath>

AIComponent::AIComponent(std::unique_ptr<AIStrategy> strategy)
    : m_strategy(std::move(strategy))
    , m_targetPlayer(nullptr)
    , m_awarenessMode(AwarenessMode::Proximity)
    , m_detectionRange(400.0f)
    , m_usePathfinding(false)
    , m_debug(false)
    , m_currentState("idle") {
    
    // Initialize target memory
    m_targetMemory.lastSeenTime = -1.0f;
}

bool AIComponent::initialize() {
    if (!m_owner) {
        std::cerr << "[ERROR] AIComponent::initialize - No owner entity set" << std::endl;
        return false;
    }
    
    // Cache required components for performance
    cacheRequiredComponents();
    
    // Initialize strategy if available
    if (m_strategy) {
        m_strategy->setOwner(m_owner);
        return true;
    }
    
    std::cerr << "[ERROR] AIComponent::initialize - No strategy provided" << std::endl;
    return false;
}

void AIComponent::update(float dt) {
    if (!m_owner || !m_strategy) return;
    
    // Update awareness of target
    updateAwareness(dt);
    
    // Update strategy behavior
    m_strategy->update(dt);
}

void AIComponent::setStrategy(std::unique_ptr<AIStrategy> strategy) {
    if (!strategy) {
        std::cerr << "[ERROR] AIComponent::setStrategy - Null strategy provided" << std::endl;
        return;
    }
    
    // Set new strategy
    m_strategy = std::move(strategy);
    
    // Initialize strategy with owner entity
    if (m_owner) {
        m_strategy->setOwner(m_owner);
    }
    
    if (m_debug) {
        std::cout << "[AIComponent] Strategy changed to " << m_strategy->getName() << std::endl;
    }
}

bool AIComponent::canDetectTarget() const {
    if (!m_targetPlayer) return false;
    
    switch (m_awarenessMode) {
        case AwarenessMode::Omniscient:
            return true;
            
        case AwarenessMode::Proximity:
            return isTargetInRange();
            
        case AwarenessMode::Visual:
            return isTargetInRange() && hasLineOfSightToTarget();
            
        case AwarenessMode::Unaware:
        default:
            return false;
    }
}

bool AIComponent::isTargetInRange() const {
    if (!m_targetPlayer || !m_transformComponent) return false;
    
    // Get positions
    sf::Vector2f myPos = m_transformComponent->getPosition();
    
    // Get target position safely
    sf::Vector2f targetPos;
    if (auto* targetTransform = m_targetPlayer->getComponent<Transform>()) {
        targetPos = targetTransform->getPosition();
    } else {
        return false;
    }
    
    // Calculate distance
    float dx = targetPos.x - myPos.x;
    float dy = targetPos.y - myPos.y;
    float distanceSquared = dx * dx + dy * dy;
    
    // Check against detection range squared (avoid sqrt for performance)
    return distanceSquared <= (m_detectionRange * m_detectionRange);
}

bool AIComponent::hasLineOfSightToTarget() const {
    if (!m_targetPlayer || !m_transformComponent) return false;
    
    // Get positions
    sf::Vector2f myPos = m_transformComponent->getPosition();
    
    // Get target position safely
    sf::Vector2f targetPos;
    if (auto* targetTransform = m_targetPlayer->getComponent<Transform>()) {
        targetPos = targetTransform->getPosition();
    } else {
        return false;
    }
    
    // TODO: Implement ray casting for proper line of sight check
    // For now, just return true (assuming no obstacles)
    return true;
}

bool AIComponent::validateDependencies() const {
    if (!m_owner) {
        std::cerr << "[ERROR] AIComponent has no owner entity" << std::endl;
        return false;
    }
    
    if (!m_strategy) {
        std::cerr << "[ERROR] AIComponent has no strategy" << std::endl;
        return false;
    }
    
    if (!m_owner->getComponent<Transform>()) {
        std::cerr << "[ERROR] Entity requires a Transform component for AIComponent" << std::endl;
        return false;
    }
    
    return true;
}

void AIComponent::updateAwareness(float dt) {
    if (!m_targetPlayer) return;
    
    bool canDetect = canDetectTarget();
    
    // Update target memory
    if (canDetect) {
        // Update last known position
        if (auto* targetTransform = m_targetPlayer->getComponent<Transform>()) {
            m_targetMemory.lastKnownPosition = targetTransform->getPosition();
            m_targetMemory.lastSeenTime = 0.0f;
        }
    } else if (m_targetMemory.lastSeenTime >= 0.0f) {
        // Increment time since last seen
        m_targetMemory.lastSeenTime += dt;
    }
    
    // Update strategy with awareness information
    if (m_strategy) {
        if (canDetect) {
            m_strategy->onTargetDetected(m_targetPlayer);
        } else if (m_targetMemory.lastSeenTime >= 0.0f && m_targetMemory.lastSeenTime < 3.0f) {
            // Target was seen recently but lost - pursue last known position
            m_strategy->onTargetLost(m_targetMemory.lastKnownPosition);
        }
    }
    
    // Debug output
    if (m_debug && canDetect) {
        std::cout << "[AIComponent] Target detected at: ("
                  << m_targetMemory.lastKnownPosition.x << ", "
                  << m_targetMemory.lastKnownPosition.y << ")" << std::endl;
    }
}

void AIComponent::cacheRequiredComponents() {
    if (!m_owner) return;
    
    // Cache transform component
    m_transformComponent = m_owner->getComponent<Transform>();
    
    // Cache physics component
    m_physicsComponent = m_owner->getComponent<PhysicsComponent>();
}
