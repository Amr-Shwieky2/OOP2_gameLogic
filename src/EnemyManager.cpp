#include "EnemyManager.h"
#include <algorithm>

void EnemyManager::update(float deltaTime, const Player& player, std::vector<std::unique_ptr<GameObject>>& allObjects) {
    extractEnemiesFromObjects(allObjects);
    updateEnemyAI(deltaTime, player);
    removeDeadEnemies();
}

void EnemyManager::extractEnemiesFromObjects(std::vector<std::unique_ptr<GameObject>>& allObjects) {
    m_enemies.clear();

    for (auto& obj : allObjects) {
        if (auto* enemy = dynamic_cast<SquareEnemy*>(obj.get())) {
            m_enemies.push_back(enemy);
        }
    }
}

void EnemyManager::updateEnemyAI(float deltaTime, const Player& player) {
    sf::Vector2f playerPos = player.getPosition();

    for (auto* enemy : m_enemies) {
        if (enemy && enemy->isAlive()) {
            enemy->followPlayer(playerPos);
        }
    }
}

void EnemyManager::removeDeadEnemies() {
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](SquareEnemy* enemy) {
                return !enemy || !enemy->isAlive();
            }),
        m_enemies.end()
    );
}

void EnemyManager::addEnemy(std::unique_ptr<SquareEnemy> enemy) {
    if (enemy) {
        m_enemies.push_back(enemy.get());
        // Note: The unique_ptr ownership should be transferred to the main game object list
        // This is just storing a reference for quick access
    }
}

void EnemyManager::render(sf::RenderTarget& target) const {
    for (const auto* enemy : m_enemies) {
        if (enemy && enemy->isAlive()) {
            enemy->render(target);
        }
    }
}
