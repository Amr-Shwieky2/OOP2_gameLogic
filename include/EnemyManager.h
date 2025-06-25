#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameObject.h"
#include "SquareEnemy.h"
#include "Player.h"
#include "FalconEnemy.h"


class EnemyManager {
public:
    EnemyManager() = default;

    void update(float deltaTime, const Player& player, std::vector<std::unique_ptr<GameObject>>& allObjects);
    void render(sf::RenderTarget& target) const;

    // Enemy management
    void addEnemy(std::unique_ptr<SquareEnemy> enemy);
    void removeDeadEnemies();
    void spawnFalconIfNeeded(float deltaTime, const Player& player, float cameraRightEdgeX);

    size_t getEnemyCount() const { return m_enemies.size(); }

private:
    std::vector<SquareEnemy*> m_enemies; // References to enemies in main object list
    std::unique_ptr<FalconEnemy> m_falcon;
    float m_falconSpawnTimer = 0.f;

    void updateEnemyAI(float deltaTime, const Player& player);
    void extractEnemiesFromObjects(std::vector<std::unique_ptr<GameObject>>& allObjects);
};
