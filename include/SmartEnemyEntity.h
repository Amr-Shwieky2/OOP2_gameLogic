#pragma once
#include "EnemyEntity.h"
#include <map>

/**
 * SmartEnemyEntity - Intelligent enemy with advanced AI decision making
 * Features: Situational analysis, predictive behavior, strategy learning, coordination
 */
class SmartEnemyEntity : public EnemyEntity {
public:
    SmartEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    void update(float dt) override;

protected:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures) override;

private:
    // Smart decision making
    void analyzeAndDecide();
    float evaluatePlayerThreat();
    float evaluateTerrainAdvantage();
    bool canSeePlayer() const;
    bool isPlayerApproaching() const;
    bool shouldRetreat() const;
    bool shouldAmbush() const;
    bool shouldHunt() const;

    // Strategy selection
    void switchToOptimalStrategy();
    void executeEmergencyBehavior();

    // State tracking
    struct GameState {
        sf::Vector2f playerPosition;
        sf::Vector2f playerVelocity;
        sf::Vector2f enemyPosition;
        sf::Vector2f lastPlayerPosition;
        float playerDistance;
        float playerHealth;
        bool playerHasShield;
        bool playerIsBoosted;
        int nearbyEnemies;
    } m_gameState;

    // Decision timers
    float m_decisionTimer = 0.0f;
    float m_decisionInterval = 0.5f; // Analyze every 0.5 seconds

    // Behavior parameters
    float m_dangerDistance = 150.0f;
    float m_huntDistance = 300.0f;
    float m_retreatDistance = 100.0f;
    float m_ambushDistance = 200.0f;

    // Current strategy tracking
    enum class SmartStrategy {
        Analyzing,
        Hunting,      // Aggressive pursuit
        Ambushing,    // Wait for player to come close
        Retreating,   // Defensive withdrawal
        Patrolling,   // Default behavior
        Flanking,     // Try to get behind player
        Coordinating  // Work with other enemies
    };

    SmartStrategy m_currentSmartStrategy = SmartStrategy::Analyzing;
    SmartStrategy m_previousStrategy = SmartStrategy::Patrolling;

    // Learning system
    struct StrategyPerformance {
        int successCount = 0;
        int failureCount = 0;
        float lastSuccessTime = 0.0f;

        float getSuccessRate() const {
            int total = successCount + failureCount;
            return total > 0 ? (float)successCount / total : 0.5f;
        }
    };

    std::map<SmartStrategy, StrategyPerformance> m_strategyHistory;
    void recordStrategyOutcome(SmartStrategy strategy, bool success);
    SmartStrategy getBestPerformingStrategy();

    // Communication with other enemies
    void communicateWithNearbyEnemies();
    bool shouldCoordinate() const;

    // Predictive behavior
    sf::Vector2f predictPlayerPosition(float timeAhead) const;
    sf::Vector2f findBestAmbushPosition() const;
    sf::Vector2f findBestFlankingPosition() const;
};