#pragma once
#include "LevelManager.h"
#include "SurpriseBoxManager.h"
#include "Player.h"
#include "ResourceManager.h"

class GameStateManager {
public:
    GameStateManager(TextureManager& textures, sf::RenderWindow& window);

    void initialize();
    void update(float deltaTime, Player& player);

    // Level management
    void loadCurrentLevel();
    const std::string& getCurrentLevelPath() const;

    // Surprise boxes
    void handleCoinCollection(int newScore);
    void setSpawnCallback(std::function<void(std::unique_ptr<GameObject>)> callback);
    void setPlayer(Player* player);

private:
    LevelManager m_levelManager;
    std::unique_ptr<SurpriseBoxManager> m_surpriseBoxManager;

    int m_lastScore = 0;
};