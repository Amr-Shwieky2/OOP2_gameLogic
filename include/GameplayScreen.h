// GameplayScreen.h - Updated Header
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "IScreen.h"
#include "GameSession.h"  // CHANGED: Use GameSession instead of GameWorld
#include "CameraManager.h"
#include "BackgroundRenderer.h"
#include "InputManager.h"
#include "UIOverlay.h"
#include "ResourceManager.h"

class GameplayScreen : public IScreen {
public:
    GameplayScreen();
    ~GameplayScreen();

    // IScreen interface
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void initializeComponents();
    void setupGameCallbacks();
    void handlePlayerInput(PlayerEntity& player);
    void updateCameraForPlayer(PlayerEntity& player);
    void updateUI(PlayerEntity& player);

    // Core components - CHANGED: GameSession instead of GameWorld
    std::unique_ptr<GameSession> m_gameSession;
    std::unique_ptr<CameraManager> m_cameraManager;
    std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<UIOverlay> m_ui;

    // Resources
    ResourceManager<sf::Texture> m_textures;
    sf::RenderWindow* m_window = nullptr;

    // Level management
    std::string m_currentLevel = "level1.txt";
    bool m_initialized = false;
};