#include "SmartEnemyEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "AIComponent.h"
#include "PatrolStrategy.h"
#include "FollowPlayerStrategy.h"
#include "GuardStrategy.h"
#include "PlayerEntity.h"
#include "PlayerState.h"
#include "GameSession.h"
#include "Constants.h"
#include <iostream>
#include <cmath>
#include <algorithm>

extern GameSession* g_currentSession;

SmartEnemyEntity::SmartEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : EnemyEntity(id, EnemyType::Smart, world, x, y, textures) {
    std::cout << "[SMART ENEMY] Creating intelligent enemy with advanced AI" << std::endl;
    setupComponents(world, x, y, textures);

    // Initialize strategy performance tracking
    for (int i = 0; i < 7; ++i) {
        m_strategyHistory[static_cast<SmartStrategy>(i)] = StrategyPerformance();
    }
}

void SmartEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    EnemyEntity::setupComponents(world, x, y, textures);

    // Position enemy at tile center
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f ;

    // Update transform position
    auto* transform = getComponent<Transform>();
    if (transform) {
        transform->setPosition(centerX, centerY);
    }

    // Add physics - larger and stronger than regular enemies
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(TILE_SIZE * 0.1f, TILE_SIZE * 0.1f,
        1.2f,    // Higher density
        0.3f,    // Same friction
        0.0f);   // No bouncing
    physics->setPosition(centerX, centerY);

    if (auto* body = physics->getBody()) {
        body->SetFixedRotation(true);
        body->SetGravityScale(1.0f);
        body->SetLinearDamping(0.1f); // Slight damping for better control
        body->SetAwake(true);
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    // Enhanced rendering - distinctive appearance
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("SquareEnemy.png"));
    auto& sprite = render->getSprite();

    // Larger and with distinctive coloring
    sprite.setScale(0.18f, 0.18f);
    sprite.setColor(sf::Color(255, 150, 150)); // Reddish tint for danger

    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite.setPosition(centerX, centerY);

    // More health - this is a boss-like enemy
    auto* health = getComponent<HealthComponent>();
    if (health) {
        health->setHealth(5); // Takes 5 hits to kill
    }

    // Start with analyzing behavior
    auto* ai = addComponent<AIComponent>(std::make_unique<PatrolStrategy>(200.0f, 60.0f));
    if (ai && g_currentSession && g_currentSession->getPlayer()) {
        ai->setTarget(g_currentSession->getPlayer());
    }
    setupEyes();

    std::cout << "[SMART ENEMY] Enhanced smart enemy setup complete" << std::endl;
}

void SmartEnemyEntity::setupEyes() {
    m_leftEye.setRadius(4.f);
    m_leftEye.setFillColor(sf::Color::Red);
    m_leftEye.setOutlineThickness(1.5f);
    m_leftEye.setOrigin(4.f, 4.f); 

    m_rightEye.setRadius(4.f);
    m_rightEye.setFillColor(sf::Color::Red);
    m_rightEye.setOutlineThickness(1.5f);
    m_rightEye.setOrigin(4.f, 4.f); 
}

void SmartEnemyEntity::updateEyePositions() {
    auto* transform = getComponent<Transform>();
    if (!transform) {
        return;
    }

    sf::Vector2f pos = transform->getPosition();

    m_leftEye.setRadius(6.f);  
    m_rightEye.setRadius(6.f);
    m_leftEye.setOrigin(6.f, 6.f);
    m_rightEye.setOrigin(6.f, 6.f);

    m_leftEye.setPosition(pos.x - 38.f, pos.y - 50.f);
    m_rightEye.setPosition(pos.x - 10.f , pos.y - 45.f);
}


void SmartEnemyEntity::update(float dt) {
    EnemyEntity::update(dt);

    m_decisionTimer += dt;

    m_blinkTimer += dt;
    if (m_blinkTimer >= m_eyeBlinkInterval) {
        m_blinkTimer = 0.0f;
        m_eyesVisible = !m_eyesVisible; 
        m_eyeBlinkInterval = 1.5f + (rand() % 3) * 0.5f;
    }

    // Make decisions at regular intervals
    if (m_decisionTimer >= m_decisionInterval) {
        analyzeAndDecide();
        m_decisionTimer = 0.0f;
    }
    updateEyePositions();

    // FORCE MOVEMENT DEBUG - Remove this after testing
    auto* ai = getComponent<AIComponent>();
    auto* physics = getComponent<PhysicsComponent>();
    if (ai && !ai->getStrategy()) {
        std::cout << "[SMART ENEMY " << getId() << "] WARNING: No AI strategy!" << std::endl;
        // Force a default strategy
        ai->setStrategy(std::make_unique<PatrolStrategy>(200.0f, 100.0f));
        if (g_currentSession && g_currentSession->getPlayer()) {
            ai->setTarget(g_currentSession->getPlayer());
        }
    }

    // Debug movement every 2 seconds
    static float debugTimer = 0.0f;
    debugTimer += dt;
    if (debugTimer >= 2.0f) {
        debugTimer = 0.0f;
        if (physics) {
            sf::Vector2f vel = physics->getVelocity();
            sf::Vector2f pos = physics->getPosition();
            std::cout << "[SMART ENEMY " << getId() << "] Pos: (" << pos.x << "," << pos.y
                << ") Vel: (" << vel.x << "," << vel.y << ") Strategy: "
                << (int)m_currentSmartStrategy << " Eyes: " << (m_eyesVisible ? "Visible" : "Hidden") << std::endl;
        }
    }

    // Update visual feedback based on current strategy
    auto* render = getComponent<RenderComponent>();
    if (render) {
        sf::Color baseColor(255, 150, 150);

        switch (m_currentSmartStrategy) {
        case SmartStrategy::Hunting:
            baseColor = sf::Color(255, 100, 100); // Bright red - aggressive
            break;
        case SmartStrategy::Ambushing:
            baseColor = sf::Color(255, 255, 100); // Yellow - waiting
            break;
        case SmartStrategy::Retreating:
            baseColor = sf::Color(100, 100, 255); // Blue - defensive
            break;
        case SmartStrategy::Flanking:
            baseColor = sf::Color(255, 100, 255); // Purple - tactical
            break;
        case SmartStrategy::Coordinating:
            baseColor = sf::Color(100, 255, 100); // Green - teamwork
            break;
        default:
            baseColor = sf::Color(255, 150, 150); // Default pink
            break;
        }

        // Add pulsing effect for activity
        float pulse = 0.8f + 0.2f * std::sin(m_decisionTimer * 8.0f);
        baseColor.r = static_cast<sf::Uint8>(baseColor.r * pulse);
        baseColor.g = static_cast<sf::Uint8>(baseColor.g * pulse);
        baseColor.b = static_cast<sf::Uint8>(baseColor.b * pulse);

        render->getSprite().setColor(baseColor);
    }
    updateEyeColors();
}

void SmartEnemyEntity::updateEyeColors() {
    if (!g_currentSession) return;

    float darkness = g_currentSession->getDarkLevelSystem().getDarknessLevel();

    sf::Uint8 intensity = static_cast<sf::Uint8>(std::min(255.0f, 150.0f + (darkness * 105.0f)));

    sf::Color eyeColor = sf::Color::Red;

    m_leftEye.setFillColor(eyeColor);
    m_rightEye.setFillColor(eyeColor);

    sf::Color outlineColor(255, 255, 255, intensity / 2);
    m_leftEye.setOutlineColor(outlineColor);
    m_rightEye.setOutlineColor(outlineColor);
}

void SmartEnemyEntity::analyzeAndDecide() {
    if (!g_currentSession || !g_currentSession->getPlayer()) {
        return;
    }

    // Update game state
    PlayerEntity* player = g_currentSession->getPlayer();
    auto* playerTransform = player->getComponent<Transform>();
    auto* playerPhysics = player->getComponent<PhysicsComponent>();
    auto* playerHealth = player->getComponent<HealthComponent>();
    auto* enemyTransform = getComponent<Transform>();

    if (!playerTransform || !enemyTransform) return;

    // Store previous position for velocity calculation
    m_gameState.lastPlayerPosition = m_gameState.playerPosition;
    m_gameState.playerPosition = playerTransform->getPosition();
    m_gameState.enemyPosition = enemyTransform->getPosition();

    if (playerPhysics) {
        m_gameState.playerVelocity = playerPhysics->getVelocity();
    }

    // Calculate distance
    sf::Vector2f diff = m_gameState.playerPosition - m_gameState.enemyPosition;
    m_gameState.playerDistance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    // Get player status
    if (playerHealth) {
        m_gameState.playerHealth = playerHealth->getHealth();
        m_gameState.playerHasShield = playerHealth->isInvulnerable();
    }

    // Check player state
    std::string playerState = "Normal";
    if (player->getCurrentState()) {
        playerState = player->getCurrentState()->getName();
        m_gameState.playerIsBoosted = (playerState == "Boosted");
    }

    // Count nearby enemies for coordination
    m_gameState.nearbyEnemies = 0;
    for (auto* entity : g_currentSession->getEntityManager().getAllEntities()) {
        if (auto* enemy = dynamic_cast<EnemyEntity*>(entity)) {
            if (enemy != this && enemy->isActive()) {
                auto* otherEnemyTransform = enemy->getComponent<Transform>();
                if (otherEnemyTransform) {
                    sf::Vector2f enemyPos = otherEnemyTransform->getPosition();
                    sf::Vector2f toEnemy = enemyPos - m_gameState.enemyPosition;
                    float distance = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);
                    if (distance < 400.0f) { // Within coordination range
                        m_gameState.nearbyEnemies++;
                    }
                }
            }
        }
    }

    // Make intelligent decision
    switchToOptimalStrategy();
}

void SmartEnemyEntity::switchToOptimalStrategy() {
    SmartStrategy newStrategy = m_currentSmartStrategy;

    // Emergency behaviors first
    if (shouldRetreat()) {
        newStrategy = SmartStrategy::Retreating;
        std::cout << "[SMART ENEMY " << getId() << "] RETREATING - Player too dangerous!" << std::endl;
    }
    else if (m_gameState.playerHealth <= 1 && m_gameState.playerDistance < m_huntDistance) {
        // Player is weak - go for the kill!
        newStrategy = SmartStrategy::Hunting;
        std::cout << "[SMART ENEMY " << getId() << "] HUNTING - Player is weak!" << std::endl;
    }
    else if (shouldAmbush()) {
        newStrategy = SmartStrategy::Ambushing;
        std::cout << "[SMART ENEMY " << getId() << "] AMBUSHING - Waiting for optimal moment" << std::endl;
    }
    else if (shouldCoordinate() && m_gameState.nearbyEnemies > 0) {
        newStrategy = SmartStrategy::Coordinating;
        std::cout << "[SMART ENEMY " << getId() << "] COORDINATING - Working with "
            << m_gameState.nearbyEnemies << " allies" << std::endl;
    }
    else if (shouldHunt()) {
        newStrategy = SmartStrategy::Hunting;
        std::cout << "[SMART ENEMY " << getId() << "] HUNTING - Player in range" << std::endl;
    }
    else if (m_gameState.playerDistance > 200.0f && canSeePlayer()) {
        newStrategy = SmartStrategy::Flanking;
        std::cout << "[SMART ENEMY " << getId() << "] FLANKING - Getting better position" << std::endl;
    }
    else {
        // Default patrol with enhanced parameters
        newStrategy = SmartStrategy::Patrolling;
    }

    // Apply strategy if changed
    if (newStrategy != m_currentSmartStrategy) {
        m_previousStrategy = m_currentSmartStrategy;
        m_currentSmartStrategy = newStrategy;

        auto* ai = getComponent<AIComponent>();
        if (ai) {
            switch (newStrategy) {
            case SmartStrategy::Hunting:
                ai->setStrategy(std::make_unique<FollowPlayerStrategy>(400.0f, 600.0f));
                break;

            case SmartStrategy::Ambushing:
                ai->setStrategy(std::make_unique<GuardStrategy>(150.0f, 200.0f));
                break;

            case SmartStrategy::Retreating:
                // Use patrol but move away from player
                ai->setStrategy(std::make_unique<PatrolStrategy>(400.0f, 150.0f));
                break;

            case SmartStrategy::Flanking:
                ai->setStrategy(std::make_unique<FollowPlayerStrategy>(180.0f, 500.0f));
                break;

            case SmartStrategy::Coordinating:
                ai->setStrategy(std::make_unique<FollowPlayerStrategy>(160.0f, 450.0f));
                break;

            default:
                ai->setStrategy(std::make_unique<PatrolStrategy>(300.0f, 100.0f));
                break;
            }

            // IMPORTANT: Make sure AI has player target!
            if (g_currentSession && g_currentSession->getPlayer()) {
                ai->setTarget(g_currentSession->getPlayer());
                std::cout << "[SMART ENEMY " << getId() << "] Strategy set with player target" << std::endl;
            }
        }
    }
}

bool SmartEnemyEntity::canSeePlayer() const {
    // Simple line-of-sight check
    return m_gameState.playerDistance < 400.0f;
}

bool SmartEnemyEntity::isPlayerApproaching() const {
    // Check if player is moving toward us
    sf::Vector2f toEnemy = m_gameState.enemyPosition - m_gameState.playerPosition;
    float dot = toEnemy.x * m_gameState.playerVelocity.x + toEnemy.y * m_gameState.playerVelocity.y;
    return dot > 0 && std::abs(m_gameState.playerVelocity.x) > 1.0f;
}

bool SmartEnemyEntity::shouldRetreat() const {
    // Retreat if player has shield and is close, or if we're low on health
    auto* health = getComponent<HealthComponent>();
    bool lowHealth = health && health->getHealth() <= 2;
    bool playerDangerous = m_gameState.playerHasShield || m_gameState.playerIsBoosted;
    bool tooClose = m_gameState.playerDistance < m_retreatDistance;

    return (lowHealth && tooClose) || (playerDangerous && tooClose);
}

bool SmartEnemyEntity::shouldAmbush() const {
    // Ambush if player is moving but not directly toward us
    bool playerMoving = std::abs(m_gameState.playerVelocity.x) > 0.5f;
    bool notApproaching = !isPlayerApproaching();
    bool goodDistance = m_gameState.playerDistance > m_retreatDistance &&
        m_gameState.playerDistance < m_ambushDistance;

    return playerMoving && notApproaching && goodDistance;
}

bool SmartEnemyEntity::shouldHunt() const {
    // Hunt if player is vulnerable and in range
    bool playerVulnerable = !m_gameState.playerHasShield && !m_gameState.playerIsBoosted;
    bool inRange = m_gameState.playerDistance < m_huntDistance && m_gameState.playerDistance > m_retreatDistance;
    bool canWin = m_gameState.playerHealth <= 3; // We can potentially win

    return playerVulnerable && inRange && canWin;
}

bool SmartEnemyEntity::shouldCoordinate() const {
    // Coordinate when there are nearby enemies and player is strong
    bool playerStrong = m_gameState.playerHealth > 2 || m_gameState.playerHasShield;
    bool hasAllies = m_gameState.nearbyEnemies > 0;
    bool playerInRange = m_gameState.playerDistance < 300.0f;

    return playerStrong && hasAllies && playerInRange;
}

sf::Vector2f SmartEnemyEntity::predictPlayerPosition(float timeAhead) const {
    return m_gameState.playerPosition + m_gameState.playerVelocity * timeAhead;
}

sf::Vector2f SmartEnemyEntity::findBestAmbushPosition() const {
    // Find a position where we can intercept the player
    sf::Vector2f predictedPos = predictPlayerPosition(2.0f);
    sf::Vector2f currentPos = m_gameState.enemyPosition;

    // Move to intercept
    sf::Vector2f interceptDir = predictedPos - currentPos;
    float length = std::sqrt(interceptDir.x * interceptDir.x + interceptDir.y * interceptDir.y);
    if (length > 0) {
        interceptDir /= length;
    }

    return currentPos + interceptDir * 150.0f;
}

sf::Vector2f SmartEnemyEntity::findBestFlankingPosition() const {
    // Try to get behind or to the side of the player
    sf::Vector2f toPlayer = m_gameState.playerPosition - m_gameState.enemyPosition;

    // Rotate 90 degrees to get a flanking position
    sf::Vector2f flankDir(-toPlayer.y, toPlayer.x);
    float length = std::sqrt(flankDir.x * flankDir.x + flankDir.y * flankDir.y);
    if (length > 0) {
        flankDir /= length;
    }

    return m_gameState.playerPosition + flankDir * 100.0f;
}

void SmartEnemyEntity::recordStrategyOutcome(SmartStrategy strategy, bool success) {
    if (success) {
        m_strategyHistory[strategy].successCount++;
        m_strategyHistory[strategy].lastSuccessTime = 0.0f; // Reset timer
    }
    else {
        m_strategyHistory[strategy].failureCount++;
    }

    std::cout << "[SMART ENEMY] Strategy " << (int)strategy
        << (success ? " succeeded" : " failed")
        << " (Success rate: " << m_strategyHistory[strategy].getSuccessRate() * 100 << "%)" << std::endl;
}

SmartEnemyEntity::SmartStrategy SmartEnemyEntity::getBestPerformingStrategy() {
    SmartStrategy best = SmartStrategy::Patrolling;
    float bestRate = 0.0f;

    for (const auto& [strategy, performance] : m_strategyHistory) {
        if (performance.getSuccessRate() > bestRate) {
            bestRate = performance.getSuccessRate();
            best = strategy;
        }
    }

    return best;
}

float SmartEnemyEntity::evaluatePlayerThreat() {
    float threat = 0.0f;

    // Base threat from player health
    threat += m_gameState.playerHealth * 20.0f;

    // Extra threat if player has power-ups
    if (m_gameState.playerHasShield) threat += 50.0f;
    if (m_gameState.playerIsBoosted) threat += 30.0f;

    // Distance factor - closer is more threatening
    if (m_gameState.playerDistance < 100.0f) threat += 40.0f;
    else if (m_gameState.playerDistance < 200.0f) threat += 20.0f;

    return threat;
}

float SmartEnemyEntity::evaluateTerrainAdvantage() {
    // Simple terrain evaluation - could be expanded
    // For now, just consider if we're at a good height relative to player
    float heightDiff = m_gameState.enemyPosition.y - m_gameState.playerPosition.y;

    // Higher ground is advantageous
    if (heightDiff > 50.0f) return 25.0f;
    if (heightDiff < -50.0f) return -15.0f; // Lower ground is disadvantageous

    return 0.0f;
}

void SmartEnemyEntity::executeEmergencyBehavior() {
    // Quick escape behavior when in immediate danger
    auto* physics = getComponent<PhysicsComponent>();
    if (!physics) return;

    // Move away from player quickly
    sf::Vector2f escapeDir = m_gameState.enemyPosition - m_gameState.playerPosition;
    float length = std::sqrt(escapeDir.x * escapeDir.x + escapeDir.y * escapeDir.y);
    if (length > 0) {
        escapeDir /= length;
        physics->setVelocity(escapeDir.x * 200.0f, physics->getVelocity().y);
    }
}

void SmartEnemyEntity::communicateWithNearbyEnemies() {
    // Could implement enemy-to-enemy communication here
    // For example, alerting nearby enemies about player position
    // This would require a messaging system between entities
}

void SmartEnemyEntity::drawEyes(sf::RenderWindow& window) {
    if (!g_currentSession) {
        return;
    }
    float darkness = g_currentSession->getDarkLevelSystem().getDarknessLevel();

    if (darkness < 0.5f || !m_eyesVisible) return;

    auto* transform = getComponent<Transform>();
    if (!transform) {
        return;
    }

    sf::Vector2f pos = transform->getPosition();

    sf::Color leftColor = m_leftEye.getFillColor();
    sf::Color rightColor = m_rightEye.getFillColor();

    m_leftEye.setRadius(6.f);
    m_rightEye.setRadius(6.f);
    m_leftEye.setFillColor(sf::Color(255, 0, 0, 255)); 
    m_rightEye.setFillColor(sf::Color(255, 0, 0, 255));
    m_leftEye.setOutlineThickness(3.f);
    m_rightEye.setOutlineThickness(3.f);

    window.draw(m_leftEye);
    window.draw(m_rightEye);

}