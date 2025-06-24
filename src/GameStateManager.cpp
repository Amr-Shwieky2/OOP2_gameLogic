#include "GameStateManager.h"

GameStateManager::GameStateManager(TextureManager& textures, sf::RenderWindow& window) {
    m_levelManager.addLevel("level1.txt");
    m_levelManager.addLevel("level2.txt");

    m_surpriseBoxManager = std::make_unique<SurpriseBoxManager>(textures, window);
}

void GameStateManager::initialize() {
    // Setup initial state
}

void GameStateManager::update(float deltaTime, Player& player) {
    // Handle surprise box triggering
    handleCoinCollection(player.getScore());
}

const std::string& GameStateManager::getCurrentLevelPath() const {
    return m_levelManager.getCurrentLevelPath();
}

void GameStateManager::handleCoinCollection(int newScore) {
    if (newScore > m_lastScore && (newScore % 10) == 0) {
        m_surpriseBoxManager->onCoinCollected();
    }
    m_lastScore = newScore;
}

void GameStateManager::setPlayer(Player* player) {
    if (m_surpriseBoxManager) {
        m_surpriseBoxManager->setPlayer(player);
    }
}

void GameStateManager::setSpawnCallback(std::function<void(std::unique_ptr<GameObject>)> callback) {
    if (m_surpriseBoxManager) {
        m_surpriseBoxManager->setSpawnCallback(callback);
    }
}

void GameStateManager::loadCurrentLevel() {
    // يمكن إضافة منطق تحميل المستوى هنا إذا احتجنا تحكم أكثر
}