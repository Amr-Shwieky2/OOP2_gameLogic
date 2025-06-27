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

void EnemyManager::spawnFalconIfNeeded(float deltaTime, const Player& player, float cameraRightEdgeX) {
    m_falconSpawnTimer += deltaTime;

    if (!m_falcon && m_falconSpawnTimer >= 5.f) {
        if (!m_hasSpawnedFirstFalcon) {
            m_showWarning = true;
            m_warningTimer = 2.f; // Show warning for 2 seconds
            m_hasSpawnedFirstFalcon = true;
            return;
        }

        m_falconSpawnTimer = 0.f;

        float spawnX = cameraRightEdgeX + 100.f; // Spawn offscreen right
        float spawnY = 200.f;
        b2World& world = *player.getBody()->GetWorld();

        m_falcon = std::make_unique<FalconEnemy>(world, spawnX, spawnY, player.getTextureManager(), 1);
    }

    if (m_falcon) {
        m_falcon->updateWithPlayer(deltaTime, const_cast<Player*>(&player));  
        m_falcon->shoot(player.getTextureManager());

        if (m_falcon->getPosition().x < player.getPosition().x - 800.f) {
            m_falcon.reset();
        }
    }

    if (m_showWarning) {
        m_warningTimer -= deltaTime;
        if (m_warningTimer <= 0.f) {
            m_showWarning = false;
        }
    }
}

void EnemyManager::loadWarningTexture(TextureManager& textures) {
    m_warningSprite.setTexture(textures.getResource("warningScreen.png"));

    sf::Vector2u size = m_warningSprite.getTexture()->getSize();
    m_warningSprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_warningSprite.setScale(1.f, 1.f);
}

void EnemyManager::addEnemy(std::unique_ptr<SquareEnemy> enemy) {
    if (enemy) {
        m_enemies.push_back(enemy.get());
    }
}

void EnemyManager::render(sf::RenderTarget& target) const {
    for (const auto* enemy : m_enemies) {
        if (enemy && enemy->isAlive()) {
            enemy->render(target);
        }
    }

    if (m_falcon) {
        m_falcon->render(target);
    }

    if (m_showWarning) {
        // Use a local copy of the sprite to modify it safely in a const method
        sf::Sprite warning = m_warningSprite;
        warning.setPosition(target.getView().getCenter());
        target.draw(warning);
    }
}
