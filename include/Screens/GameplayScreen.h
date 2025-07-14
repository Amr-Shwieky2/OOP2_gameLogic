#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "IScreen.h"
#include "GameSession.h"
#include "CameraManager.h"
#include "BackgroundRenderer.h"
#include "InputService.h"
#include "UIOverlay.h"
#include "ResourceManager.h"
#include "Entity.h"  // Added for Entity class
#include <DarkLevelSystem.h>

// Forward declarations
class UIObserver;
class WellEnteredEvent;
class LevelTransitionEvent;
class PlayerEntity;
class Transform;
class HealthComponent;
class PhysicsComponent;

/**
 * @class GameplayScreen
 * @brief Main gameplay screen handling rendering, input and game state
 */
class GameplayScreen : public IScreen {
public:
    GameplayScreen();
    ~GameplayScreen() override;

    // IScreen interface
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    // Lifecycle hooks
    void onEnter() override {}
    void onExit() override {}

private:
    // Core systems
    std::unique_ptr<GameSession> m_gameSession;
    std::unique_ptr<CameraManager> m_cameraManager;
    std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;
    std::unique_ptr<UIOverlay> m_ui;
    std::unique_ptr<UIObserver> m_uiObserver;
    std::unique_ptr<DarkLevelSystem> m_darkLevelSystem;
    
    // Input handling
    InputService m_inputService;
    
    // Resources
    ResourceManager<sf::Texture> m_textures;
    sf::RenderWindow* m_window = nullptr;
    sf::Font m_font;
    
    // UI elements
    sf::Text m_levelCompleteText;
    sf::Text m_gameCompleteText;
    sf::Text m_gameOverText;
    sf::RectangleShape m_messageBackground;
    sf::RectangleShape m_gameOverBackground;
    sf::Sprite m_gameOverSprite;
    
    // State flags
    bool m_initialized = false;
    bool m_isUnderground = false;
    bool m_showingLevelComplete = false;
    bool m_showingGameComplete = false;
    bool m_showingGameOver = false;
    float m_messageTimer = 0.0f;
    float m_messageDuration = 3.0f;
    bool m_levelTransitionInProgress = false;  // Added for safe level transitions
    bool m_playerValid = false;                // Added for player validation
    
    // Initialization methods
    void initializeComponents();
    void initializeUITexts();
    void initializeBackgrounds();
    void loadGameOverSprite();
    void initializeUIObserver();
    void initializeGameSession(sf::RenderWindow& window);

    // Input handling
    void handleKeyboardInput(sf::Keyboard::Key keyCode);
    void handlePlayerInput(PlayerEntity& player);
    void handleDebugKeys(PlayerEntity& player);

    // Update methods
    void updateGameState(float deltaTime, PlayerEntity& player);
    void updateCameraForPlayer(PlayerEntity& player);
    void updateUI(PlayerEntity& player);
    void updateMessageTimers(float deltaTime);
    void checkGameOverCondition(PlayerEntity* player);

    // Level handling
    bool handleWellLevelChangeRequests();
    void activateDarkLevelIfNeeded(const std::string& levelName);
    bool startLevelTransition(const std::string& targetLevel);
    void registerShadowCastingObjects();

    // Event handling
    void setupLevelEventHandlers();
    void handleWellEnteredEvent(const WellEnteredEvent& event);
    void onLevelTransition(const LevelTransitionEvent& event);
    void showLevelCompleteMessage();
    void showGameCompleteMessage();
    bool isPlayerValid(PlayerEntity* player);
    
    // Rendering methods
    void renderGameMessages(sf::RenderWindow& window);
    
    // Component safety helpers
    template <typename T>
    T* getSafeComponent(Entity* entity) const;
};