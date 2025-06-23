#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "IScreen.h"
#include "InputService.h"
#include "VoiceInputService.h"
#include "Player.h"
#include "Map.h"
#include "ResourceManager.h"
#include "LevelManager.h"
#include "UIOverlay.h"
#include "CollisionSystem.h"
#include "SurpriseBoxManager.h"
#include "CameraController.h"
#include "GameState.h"
#include "Constants.h"

class GameplayScreen : public IScreen {
private:
    // Core game systems - each with single responsibility
    std::unique_ptr<b2World> m_world;
    std::unique_ptr<GameState> m_gameState;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Map> m_map;
    std::unique_ptr<CameraController> m_camera;
    std::unique_ptr<CollisionSystem> m_collisionSystem;
    std::unique_ptr<UIOverlay> m_ui;
    std::unique_ptr<SurpriseBoxManager> m_surpriseBoxManager;

    // Managers and services
    LevelManager m_levelManager;
    InputService m_input;
    VoiceInputService m_voiceInput;
    ResourceManager<sf::Texture> m_textures;

    // Rendering components
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    // Window reference
    sf::RenderWindow* m_window = nullptr;

public:
    GameplayScreen();
    ~GameplayScreen();

    // IScreen interface
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    // Initialization methods - clear single responsibilities
    void initializeWorld();
    void initializeGameState();
    void initializeBackground();
    void initializeLevels();
    void initializePlayer();
    void initializeSystems();
    void initializeUI();

    // Update methods - separated concerns
    void updateInput(float deltaTime);
    void updatePhysics(float deltaTime);
    void updateGameLogic(float deltaTime);
    void updateSystems(float deltaTime);
    void checkGameEvents();

    // Render methods - clean separation
    void renderBackground(sf::RenderWindow& window);
    void renderGameWorld(sf::RenderWindow& window);
    void renderUI(sf::RenderWindow& window);

    // Helper methods
    void loadCurrentLevel();
    void spawnGameObject(std::unique_ptr<GameObject> obj);
    void onCoinCollected();
    void handleLevelComplete();
    void handleGameOver();
};