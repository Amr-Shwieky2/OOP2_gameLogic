#include "GameEventCoordinator.h"
#include <iostream>

//-------------------------------------------------------------------------------------
GameEventCoordinator::GameEventCoordinator() {
    std::cout << "[GameEventCoordinator] Created" << std::endl;
}
//-------------------------------------------------------------------------------------
GameEventCoordinator::~GameEventCoordinator() {
    shutdown();
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::initialize() {
    if (m_initialized) return;

    setupDefaultHandlers();
    m_initialized = true;
    std::cout << "[GameEventCoordinator] Initialized" << std::endl;
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::shutdown() {
    if (!m_initialized) return;

    m_levelCompleteHandler = nullptr;
    m_playerDeathHandler = nullptr;
    m_initialized = false;
    std::cout << "[GameEventCoordinator] Shutdown" << std::endl;
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::publishPlayerDeath(Entity::IdType playerId) {
    std::cout << "[GameEventCoordinator] Publishing player death: " << playerId << std::endl;
    EventSystem::getInstance().publish(PlayerDiedEvent(playerId));
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::publishLevelComplete(const std::string& levelName, int score, float time) {
    std::cout << "[GameEventCoordinator] Publishing level complete: " << levelName << std::endl;
    EventSystem::getInstance().publish(LevelCompletedEvent(levelName, score, time));
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::publishScoreChange(int newScore, int delta) {
    EventSystem::getInstance().publish(ScoreChangedEvent(newScore, delta));
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::publishEnemyKilled(Entity::IdType enemyId, Entity::IdType killerId) {
    EventSystem::getInstance().publish(EnemyKilledEvent(enemyId, killerId));
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::publishItemCollected(Entity::IdType playerId, Entity::IdType itemId) {
    EventSystem::getInstance().publish(ItemCollectedEvent(playerId, itemId, ItemCollectedEvent::ItemType::Coin));
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::setLevelCompleteHandler(std::function<void(const LevelCompletedEvent&)> handler) {
    m_levelCompleteHandler = handler;
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::setPlayerDeathHandler(std::function<void(const PlayerDiedEvent&)> handler) {
    m_playerDeathHandler = handler;
}
//-------------------------------------------------------------------------------------
void GameEventCoordinator::setupDefaultHandlers() {
    EventSystem::getInstance().subscribe<LevelCompletedEvent>(
        [this](const LevelCompletedEvent& event) {
            if (m_levelCompleteHandler) {
                m_levelCompleteHandler(event);
            }
        }
    );

    EventSystem::getInstance().subscribe<PlayerDiedEvent>(
        [this](const PlayerDiedEvent& event) {
            if (m_playerDeathHandler) {
                m_playerDeathHandler(event);
            }
        }
    );
}
//-------------------------------------------------------------------------------------