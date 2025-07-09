#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "IScreen.h"
#include "GameSession.h"  // Now uses SRP-compliant GameSession
#include "CameraManager.h"
#include "BackgroundRenderer.h"
#include "InputService.h"
#include "UIOverlay.h"
#include "ResourceManager.h"
#include <DarkLevelSystem.h>

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
    std::unique_ptr<DarkLevelSystem> m_darkLevelSystem;
    bool m_isUnderground = false;

    void initializeComponents();
    void handlePlayerInput(PlayerEntity& player);
    void updateCameraForPlayer(PlayerEntity& player);
    void updateUI(PlayerEntity& player);
    void initializeUIObserver();

    void setupLevelEventHandlers();
    void onLevelTransition(const LevelTransitionEvent& event);
    void showLevelCompleteMessage();
    void showGameCompleteMessage();

    // Core components - Updated to work with SRP GameSession
    std::unique_ptr<GameSession> m_gameSession;
    std::unique_ptr<CameraManager> m_cameraManager;
    std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;
    std::unique_ptr<UIOverlay> m_ui;
    std::unique_ptr<UIObserver> m_uiObserver;

    // Input handling
    InputService m_inputService;

    // UI state
    bool m_showingLevelComplete = false;
    bool m_showingGameComplete = false;
    bool m_showingGameOver = false;
    float m_messageTimer = 0.0f;
    float m_messageDuration = 3.0f;

    // UI elements
    sf::Text m_levelCompleteText;
    sf::Text m_gameCompleteText;
    sf::Text m_gameOverText;
    sf::RectangleShape m_messageBackground;
    sf::RectangleShape m_gameOverBackground;
    sf::Sprite m_gameOverSprite;

    // Resources
    ResourceManager<sf::Texture> m_textures;
    sf::RenderWindow* m_window = nullptr;
    sf::Font m_font;

    // Game state
    bool m_initialized = false;
};