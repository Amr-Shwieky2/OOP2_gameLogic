#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "IScreen.h"
#include "GameSession.h"  // Using GameSession instead of GameWorld
#include "CameraManager.h"
#include "BackgroundRenderer.h"
#include "InputService.h"  // Changed from InputManager
#include "UIOverlay.h"
#include "ResourceManager.h"

// Forward declaration to fix the error
class UIObserver;

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
    void handlePlayerInput(PlayerEntity& player);
    void updateCameraForPlayer(PlayerEntity& player);
    void updateUI(PlayerEntity& player);
    void initializeUIObserver();

    void setupLevelEventHandlers();
    void onLevelTransition(const LevelTransitionEvent& event);
    void showLevelCompleteMessage();
    void showGameCompleteMessage();

    // Core components - Using new ECS system
    std::unique_ptr<GameSession> m_gameSession;
    std::unique_ptr<CameraManager> m_cameraManager;
    std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;
    std::unique_ptr<UIOverlay> m_ui;
    std::unique_ptr<UIObserver> m_uiObserver;  // Now properly declared

    // Input handling
    InputService m_inputService;  // Direct input service instead of InputManager

    bool m_showingLevelComplete = false;
    bool m_showingGameComplete = false;
    float m_messageTimer = 0.0f;
    float m_messageDuration = 3.0f;
    sf::Text m_levelCompleteText;
    sf::Text m_gameCompleteText;
    sf::RectangleShape m_messageBackground;

    // Resources
    ResourceManager<sf::Texture> m_textures;
    sf::RenderWindow* m_window = nullptr;
    sf::Font m_font;

    // Level management
    std::string m_currentLevel = "level1.txt";
    bool m_initialized = false;
};