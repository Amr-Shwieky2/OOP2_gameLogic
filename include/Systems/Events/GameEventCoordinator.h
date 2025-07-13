#pragma once
#include "EventSystem.h"
#include "GameEvents.h"
#include <functional>

/**
 * GameEventCoordinator - Single Responsibility: Coordinate game events
 */
class GameEventCoordinator {
public:
    GameEventCoordinator();
    ~GameEventCoordinator();

    void initialize();
    void shutdown();

    // Event publishing helpers
    void publishPlayerDeath(Entity::IdType playerId);
    void publishLevelComplete(const std::string& levelName, int score, float time);
    void publishScoreChange(int newScore, int delta);
    void publishEnemyKilled(Entity::IdType enemyId, Entity::IdType killerId);
    void publishItemCollected(Entity::IdType playerId, Entity::IdType itemId);

    // Event handler registration
    void setLevelCompleteHandler(std::function<void(const LevelCompletedEvent&)> handler);
    void setPlayerDeathHandler(std::function<void(const PlayerDiedEvent&)> handler);

private:
    bool m_initialized = false;
    std::function<void(const LevelCompletedEvent&)> m_levelCompleteHandler;
    std::function<void(const PlayerDiedEvent&)> m_playerDeathHandler;

    void setupDefaultHandlers();
};