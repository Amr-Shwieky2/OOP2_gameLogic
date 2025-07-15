#include "../Screens/GameplayScreen.h"
#include "Constants.h"
#include "PlayerEntity.h"
#include "HealthComponent.h"
#include "Transform.h"
#include "UIObserver.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include "ResourcePaths.h"
#include "PhysicsComponent.h"
#include <iostream>
#include <format>
#include <WellEntity.h>
#include <typeinfo>
#include "../Core/AudioManager.h"
#include "../UI/GameOverScreen.h"
#include <Application/AppContext.h>
#include <Config/ScreenTypes.h>

// Define PIXEL_PER_METER if not already defined in Constants.h
#ifndef PIXEL_PER_METER
#define PIXEL_PER_METER 30.0f
#endif

// Utility template method for safely getting components
template <typename T>
T* GameplayScreen::getSafeComponent(Entity* entity) const {
    if (!entity) return nullptr;
    return entity->getComponent<T>();
}

/**
 * GameplayScreen constructor - initializes all needed components
 * Following Single Responsibility Principle by delegating initialization
 * to specialized methods
 */
GameplayScreen::GameplayScreen() : 
    m_initialized(false),
    m_isUnderground(false),
    m_showingLevelComplete(false),
    m_showingGameComplete(false),
    m_showingGameOver(false),
    m_messageTimer(0.0f),
    m_messageDuration(3.0f),
    m_levelTransitionInProgress(false),
    m_playerValid(false) {
    initializeComponents();
    AudioManager::instance().stopAllSounds();
    AudioManager::instance().playMusic("gameplay", true);
}

/**
 * Default destructor - smart pointers handle cleanup automatically
 * following RAII principle
 */
GameplayScreen::~GameplayScreen() = default;

/**
 * Initialize all screen components using dependency injection pattern
 * Each component has a clear single responsibility
 */
void GameplayScreen::initializeComponents() {
    try {
        // Create core systems - each with single responsibility (SRP)
        m_gameSession = std::make_unique<GameSession>();
        m_cameraManager = std::make_unique<CameraManager>();
        m_backgroundRenderer = std::make_unique<BackgroundRenderer>(m_textures);
        m_ui = std::make_unique<UIOverlay>(WINDOW_WIDTH);
        m_darkLevelSystem = std::make_unique<DarkLevelSystem>();

        // Load font for UI
        if (!m_font.loadFromFile("arial.ttf")) {
            std::cerr << "[WARNING] Failed to load font" << std::endl;
        }

        // Initialize camera
        m_cameraManager->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

        // Setup UI components with consistent styling
        initializeUITexts();
        
        // Setup backgrounds with proper layering
        initializeBackgrounds();

        // Try to load Game Over sprite with proper exception handling
        loadGameOverSprite();

        std::cout << "[GameplayScreen] Components initialized with SRP GameSession" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to initialize GameplayScreen components: " << e.what() << std::endl;
        throw; // Rethrow to be handled by App
    }
}

/**
 * Set up all UI text elements with consistent styling
 * Extracted to separate method to keep initializeComponents cleaner
 */
void GameplayScreen::initializeUITexts() {
    // Level complete text
    m_levelCompleteText.setFont(m_font);
    m_levelCompleteText.setCharacterSize(48);
    m_levelCompleteText.setFillColor(sf::Color::Yellow);
    m_levelCompleteText.setOutlineThickness(3.0f);
    m_levelCompleteText.setOutlineColor(sf::Color::Black);
    m_levelCompleteText.setString("Level Complete!");

    // Game complete text
    m_gameCompleteText.setFont(m_font);
    m_gameCompleteText.setCharacterSize(54);
    m_gameCompleteText.setFillColor(sf::Color::Yellow);
    m_gameCompleteText.setOutlineThickness(3.0f);
    m_gameCompleteText.setOutlineColor(sf::Color::Black);
    m_gameCompleteText.setString("Game Complete! Congratulations!");

    // Game over text
    m_gameOverText.setFont(m_font);
    m_gameOverText.setCharacterSize(32);
    m_gameOverText.setFillColor(sf::Color::White);
    m_gameOverText.setOutlineThickness(2.0f);
    m_gameOverText.setOutlineColor(sf::Color::Black);
    m_gameOverText.setString("Press SPACE to restart");
}

/**
 * Set up all background elements
 * Extracted to separate method for better organization
 */
void GameplayScreen::initializeBackgrounds() {
    // Message background
    m_messageBackground.setSize(sf::Vector2f(WINDOW_WIDTH, 200.0f));
    m_messageBackground.setFillColor(sf::Color(0, 0, 0, 180));
    m_messageBackground.setPosition(0, WINDOW_HEIGHT / 2 - 100);

    // Game over background
    m_gameOverBackground.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    m_gameOverBackground.setFillColor(sf::Color(0, 0, 0, 150));
}

/**
 * Load game over sprite with proper error handling
 */
void GameplayScreen::loadGameOverSprite() {
    try {
        m_gameOverSprite.setTexture(m_textures.getResource("GameOver.png"));
        sf::Vector2u texSize = m_gameOverSprite.getTexture()->getSize();
        m_gameOverSprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
        m_gameOverSprite.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 50.0f);
    }
    catch (const std::exception& e) {
        std::cout << "[WARNING] Could not load GameOver.png: " << e.what() << std::endl;
    }
}

/**
 * Initialize the UI observer with proper error handling
 * Uses lazy initialization pattern - only creates if needed
 */
void GameplayScreen::initializeUIObserver() {
    if (m_font.getInfo().family.empty()) {
        std::cout << "[WARNING] Cannot initialize UIObserver: font not loaded" << std::endl;
        return;
    }

    try {
        m_uiObserver = std::make_unique<UIObserver>(m_font);
        m_uiObserver->initialize();
        std::cout << "[GameplayScreen] UI Observer initialized" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to initialize UIObserver: " << e.what() << std::endl;
    }
}

/**
 * Handle all window events and user input
 * @param window The render window
 */
void GameplayScreen::handleEvents(sf::RenderWindow& window) {
    m_window = &window;

    // One-time initialization with lazy loading pattern
    if (!m_initialized) {
        initializeGameSession(window);
    }

    // Update input service
    m_inputService.update();

    // Process all pending events
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        // Let UI handle its events first (event bubbling pattern)
        m_ui->handleEvent(event, window);

        // Handle keyboard events
        if (event.type == sf::Event::KeyPressed) {
            handleKeyboardInput(event.key.code);
        }
    }
}

/**
 * Initialize the game session and related components
 * @param window The render window
 */
void GameplayScreen::initializeGameSession(sf::RenderWindow& window) {
    try {
        // Initialize the SRP-compliant GameSession
        m_gameSession->initialize(m_textures, window);

        // Load initial level using the level manager
        m_gameSession->loadLevel(ResourcePaths::LEVEL1);

        // Initialize UI Observer
        initializeUIObserver();
        setupLevelEventHandlers();

        // Initialize dark level system
        m_darkLevelSystem->initialize(window);

        m_initialized = true;
        std::cout << "[GameplayScreen] Initialized with SRP GameSession" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to initialize game session: " << e.what() << std::endl;
        throw; // Rethrow for the App to handle
    }
}

/**
 * Handle keyboard input events
 * @param keyCode The key code pressed
 */
void GameplayScreen::handleKeyboardInput(sf::Keyboard::Key keyCode) {
    switch (keyCode) {
    case sf::Keyboard::F1:
        // Reset to first level
        m_gameSession->loadLevel(ResourcePaths::LEVEL1);
        std::cout << "[GameplayScreen] Reset to first level" << std::endl;
        break;
    case sf::Keyboard::F2:
        // Load next level
        if (m_gameSession->loadNextLevel()) {
            std::cout << "[GameplayScreen] Manually switched to next level" << std::endl;
        }
        break;
    
    case sf::Keyboard::Space:
        if (m_showingGameOver) {
            std::cout << "[GameplayScreen] Restarting level after Game Over..." << std::endl;
            m_showingGameOver = false;
            m_gameSession->reloadCurrentLevel();
        }
        break;
    case sf::Keyboard::Escape:
        if (m_window) {
            m_window->close();
        }
        break;
    default:
        break;
    }
}

/**
 * Update game state
 * @param deltaTime Time elapsed since last frame
 */
void GameplayScreen::update(float deltaTime) {
    // Don't update if the game is paused
    if (m_ui->isPaused()) return;

    // Check for level change requests from well
    if (handleWellLevelChangeRequests()) {
        return; // Exit update loop after loading level
    }

    // Update message timers
    updateMessageTimers(deltaTime);

    // Handle game over state
    if (m_showingGameOver) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            std::cout << "[GameplayScreen] Restarting level after Game Over..." << std::endl;
            m_showingGameOver = false;
            m_gameSession->reloadCurrentLevel();
        }
        return;
    }

    // Get player from the SRP-compliant GameSession
    PlayerEntity* player = m_gameSession ? m_gameSession->getPlayer() : nullptr;
    
    // Safety check - if there's no player, we can't update player-related state
    if (!player) {
        std::cout << "[WARNING] No player found in GameSession" << std::endl;
        
        // Even without player, we should update the game session 
        // to allow systems to initialize properly
        if (m_gameSession) {
            m_gameSession->update(deltaTime);
        }
        
        // Update UI observer even without player
        if (m_uiObserver) {
            m_uiObserver->update(deltaTime);
        }
        
        return;
    }

    // Handle player input and update game state
    try {
        // Handle player input
        handlePlayerInput(*player);
        
        // Update the GameSession
        if (m_gameSession) {
            m_gameSession->update(deltaTime);
        }
        
        // Re-fetch player after game session update
        player = m_gameSession ? m_gameSession->getPlayer() : nullptr;
        
        // Exit safely if player was deleted during update
        if (!player) {
            return;
        }
        
        // Update camera and UI if player is valid
        if (player && getSafeComponent<Transform>(player)) {
            updateCameraForPlayer(*player);
            updateUI(*player);
        }
        
        // Update UI observer
        if (m_uiObserver) {
            m_uiObserver->update(deltaTime);
        }
        
        // Update dark level system
        if (m_darkLevelSystem && m_isUnderground) {
            m_darkLevelSystem->update(deltaTime, player);
            
            // Periodically update obstacle positions for moving objects
            static float obstacleUpdateTimer = 0.0f;
            obstacleUpdateTimer += deltaTime;
            
            if (obstacleUpdateTimer > 1.0f) { // Update every second
                registerShadowCastingObjects();
                obstacleUpdateTimer = 0.0f;
            }
        }
        
        // Check game over condition with the latest player reference
        if (player) {
            checkGameOverCondition(player);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception in update loop: " << e.what() << std::endl;
    }
}

/**
 * Register objects that should cast shadows in the dark level
 */
void GameplayScreen::registerShadowCastingObjects() {
    if (!m_darkLevelSystem) {
        return;
    }
    
    // Clear existing obstacles
    m_darkLevelSystem->clearObstacles();
    
    // Create a simplified set of obstacles for the dark level
    std::vector<sf::FloatRect> obstacles;
    
    // Add some fixed obstacles for now
    obstacles.push_back(sf::FloatRect(100, 300, 200, 50));  // Ground platform
    obstacles.push_back(sf::FloatRect(400, 250, 150, 30));  // Floating platform
    obstacles.push_back(sf::FloatRect(700, 400, 300, 100)); // Large obstacle
    
    // Register obstacles with the dark level system
    if (!obstacles.empty()) {
        m_darkLevelSystem->setObstacles(obstacles);
        std::cout << "[GameplayScreen] Registered " << obstacles.size() << " shadow casting objects" << std::endl;
    }
}

/**
 * Handle level change requests from wells
 * @return true if a level change was processed
 */
bool GameplayScreen::handleWellLevelChangeRequests() {
    // Early exit if no level change is requested or if we're already showing transition messages
    // or if a level transition is already in progress
    if (!WellEntity::isLevelChangeRequested() || 
        m_showingLevelComplete || 
        m_showingGameComplete || 
        m_levelTransitionInProgress) {
        return false;
    }

    // Get target level and clear the request early to prevent repeated processing
    std::string targetLevel = WellEntity::getTargetLevelName();
    WellEntity::clearLevelChangeRequest();

    std::cout << "[GameplayScreen] Level change requested from well: " << targetLevel << std::endl;
    
    // Use our safe level transition method
    return startLevelTransition(targetLevel);
}

/**
 * Activate dark level system if needed based on level name
 * @param levelName The name of the loaded level
 */
void GameplayScreen::activateDarkLevelIfNeeded(const std::string& levelName) {
    std::cout << "[DEBUG] Checking level name for dark activation: '" << levelName << "'" << std::endl;
    
    bool isDarkLevel = (levelName.find("dark") != std::string::npos || 
                        levelName.find("underground") != std::string::npos);
    
    std::cout << "[DEBUG] Is dark level: " << (isDarkLevel ? "YES" : "NO") << std::endl;
    
    if (isDarkLevel) {
        std::cout << "[DEBUG] Activating dark level system..." << std::endl;

        if (m_darkLevelSystem) {
            m_darkLevelSystem->setEnabled(true);
            m_darkLevelSystem->setDarknessLevel(0.92f);
            m_isUnderground = true;

            std::cout << "[DEBUG] Dark level system enabled: " << m_darkLevelSystem->isEnabled() << std::endl;
            std::cout << "[DEBUG] Underground flag set: " << m_isUnderground << std::endl;
            std::cout << "[DEBUG] Darkness level: " << m_darkLevelSystem->getDarknessLevel() << std::endl;

            // Clear any existing light sources
            m_darkLevelSystem->clearLightSources();
            
            // Add ambient light sources for the dark level
            m_darkLevelSystem->addLightSource(sf::Vector2f(300, 400), 120.0f, sf::Color(255, 200, 100, 100)); // Warm torch
            m_darkLevelSystem->addLightSource(sf::Vector2f(800, 300), 80.0f, sf::Color(100, 180, 255, 100)); // Cool blue light

            // Register obstacles for shadow casting
            registerShadowCastingObjects();

            std::cout << "[GameplayScreen] Dark level system activated!" << std::endl;
        } else {
            std::cout << "[ERROR] Dark level system is null!" << std::endl;
        }
    } else {
        std::cout << "[DEBUG] Normal level - disabling dark effects" << std::endl;
        // Reset dark level settings for normal levels
        if (m_darkLevelSystem) {
            m_darkLevelSystem->setEnabled(false);
            m_isUnderground = false;
        }
    }
}

/**
 * Update message timers
 * @param deltaTime Time elapsed since last frame
 */
void GameplayScreen::updateMessageTimers(float deltaTime) {
    if (m_showingLevelComplete || m_showingGameComplete) {
        m_messageTimer += deltaTime;
        if (m_messageTimer >= m_messageDuration) {
            m_showingLevelComplete = false;
            m_showingGameComplete = false;
            m_messageTimer = 0.0f;
        }
    }
}

/**
 * Update the game state for a valid player entity
 * @param deltaTime Time elapsed since last frame
 * @param player Reference to the player entity
 */
void GameplayScreen::updateGameState(float deltaTime, PlayerEntity& player) {
    // Handle player input first (before anything that might destroy the player)
    try {
        // Handle player input
        handlePlayerInput(player);
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception handling player input: " << e.what() << std::endl;
    }

    // Update the SRP-compliant GameSession (coordinates all managers)
    if (m_gameSession) {
        m_gameSession->update(deltaTime);
    }

    // Player might be destroyed during update, so refetch it
    PlayerEntity* updatedPlayer = m_gameSession ? m_gameSession->getPlayer() : nullptr;

    // Only update camera and UI if player still exists
    if (updatedPlayer && updatedPlayer->hasComponent<Transform>()) {
        try {
            updateCameraForPlayer(*updatedPlayer);
            updateUI(*updatedPlayer);
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception updating camera/UI: " << e.what() << std::endl;
        }
    }

    // Update UI observer - doesn't depend on player
    if (m_uiObserver) {
        m_uiObserver->update(deltaTime);
    }

    // Update dark level system if it exists
    if (m_darkLevelSystem) {
        // Re-get player - it might have changed again
        PlayerEntity* darkLevelPlayer = m_gameSession ? m_gameSession->getPlayer() : nullptr;
        m_darkLevelSystem->update(deltaTime, darkLevelPlayer);
    }
}

/**
 * Check for game over condition
 * @param player Pointer to the player entity
 */
void GameplayScreen::checkGameOverCondition(PlayerEntity* player) {
    // Safety check - don't proceed if player is nullptr or invalid
    if (!isPlayerValid(player)) {
        return;
    }
    
    // Safely check for health component using our helper
    auto* health = getSafeComponent<HealthComponent>(player);
    
    // Only process game over if player has health and is not alive
    if (health && !health->isAlive() && !m_showingGameOver) {
        m_showingGameOver = true;

        if (auto* scoreManager = player->getScoreManager()) {
            scoreManager->resetScore();
        }

        if (m_ui) {
            m_ui->reset();
        }

        // Position the game over text
        sf::FloatRect bounds = m_gameOverText.getLocalBounds();
        m_gameOverText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        m_gameOverText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 320.0f);

        std::cout << "[GameplayScreen] Game over state activated" << std::endl;

        if (m_window) {
            GameOverScreen screen;
            screen.show(*m_window);
            AppContext::instance().screenManager().requestScreenChange(ScreenType::MENU);
        }
    }
}

/**
 * Render the game screen
 * Follows a layered rendering approach for proper z-ordering
 * @param window The render window
 */
void GameplayScreen::render(sf::RenderWindow& window) {
    // Set camera view
    m_cameraManager->setView(window);

    // Render background
    m_backgroundRenderer->render(window, m_cameraManager->getCamera());

    // Render game session (uses SRP RenderSystem internally)
    m_gameSession->render(window);

    // Debug: Print dark level system status occasionally
    static int frameCount = 0;
    frameCount++;
    if (frameCount % 300 == 0) { // Every 5 seconds at 60 FPS
        std::cout << "[DEBUG RENDER] Frame " << frameCount 
                  << " - Dark system: " << (m_darkLevelSystem ? "exists" : "null")
                  << ", Underground: " << (m_isUnderground ? "yes" : "no");
        if (m_darkLevelSystem) {
            std::cout << ", Enabled: " << (m_darkLevelSystem->isEnabled() ? "yes" : "no");
        }
        std::cout << std::endl;
    }

    // Render darkness system if in underground level
    if (m_darkLevelSystem && m_isUnderground) {
        if (frameCount % 300 == 0) {
            std::cout << "[DEBUG RENDER] Rendering dark level effects" << std::endl;
        }
        
        // Update player light position before rendering - CRITICAL for proper positioning
        PlayerEntity* player = m_gameSession ? m_gameSession->getPlayer() : nullptr;
        if (player) {
            // Force update player light position every frame
            m_darkLevelSystem->updatePlayerLight(player);
            
            auto* transform = getSafeComponent<Transform>(player);
            if (transform) {
                // Update flashlight direction based on player movement or mouse
                sf::Vector2f playerPos = transform->getPosition();
                
                // If mouse input is available, use it for flashlight direction
                if (m_window) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
                    sf::Vector2f worldPos = m_window->mapPixelToCoords(mousePos);
                    m_darkLevelSystem->updateFlashlightDirection(playerPos, worldPos);
                }
                
                // Debug output for position verification
                if (frameCount % 300 == 0) {
                    std::cout << "[DEBUG RENDER] Player position: (" << playerPos.x << ", " << playerPos.y << ")" << std::endl;
                }
            }
        }
        
        m_darkLevelSystem->render(window);
        m_darkLevelSystem->drawRedEyes(window, m_gameSession->getEntityManager());
    }

    // Switch to UI view
    sf::View defaultView = window.getDefaultView();
    window.setView(defaultView);

    // Render UI elements
    m_ui->draw(window);

    // Render UI observer notifications
    if (m_uiObserver) {
        m_uiObserver->render(window);
    }

    // Render appropriate messages based on game state
    renderGameMessages(window);
}

/**
 * Render game messages (level complete, game complete, game over)
 * @param window The render window
 */
void GameplayScreen::renderGameMessages(sf::RenderWindow& window) {
    if (m_showingLevelComplete) {
        window.draw(m_messageBackground);

        // Animate text with pulsing effect using sine wave
        float alpha = 0.8f + 0.2f * std::sin(m_messageTimer * 8.0f);
        sf::Color color = m_levelCompleteText.getFillColor();
        color.a = static_cast<sf::Uint8>(255 * alpha);
        m_levelCompleteText.setFillColor(color);

        window.draw(m_levelCompleteText);
    }

    if (m_showingGameComplete) {
        window.draw(m_messageBackground);
        window.draw(m_gameCompleteText);
    }

    if (m_showingGameOver) {
        window.draw(m_gameOverBackground);
        window.draw(m_gameOverSprite);
        window.draw(m_gameOverText);
    }
}

/**
 * Handle player input
 * @param player Reference to the player entity
 */
void GameplayScreen::handlePlayerInput(PlayerEntity& player) {
    // Player input handling
    try {
        // Player input handling
        player.handleInput(m_inputService);
        
        // Debug keys
        handleDebugKeys(player);
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception handling player input: " << e.what() << std::endl;
    }
}

/**
 * Handle debug keys for development/testing
 * @param player Reference to the player entity
 */
void GameplayScreen::handleDebugKeys(PlayerEntity& player) {
    // Score manipulation
    if (m_inputService.isKeyPressed(sf::Keyboard::F3)) {
        if (auto* scoreManager = player.getScoreManager()) {
            try {
                scoreManager->addScore(100);
                std::cout << "[DEBUG] Added 100 score" << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception adding score: " << e.what() << std::endl;
            }
        }
    }

    // Health manipulation
    if (m_inputService.isKeyPressed(sf::Keyboard::F4)) {
        auto* health = getSafeComponent<HealthComponent>(&player);
        if (health) {
            try {
                health->takeDamage(1);
                std::cout << "[DEBUG] Player health: " << health->getHealth() << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception manipulating health: " << e.what() << std::endl;
            }
        }
    }

    // Speed boost
    if (m_inputService.isKeyPressed(sf::Keyboard::F5)) {
        if (auto* stateManager = player.getStateManager()) {
            try {
                stateManager->applySpeedBoost(5.0f);
                std::cout << "[DEBUG] Applied speed boost" << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception applying speed boost: " << e.what() << std::endl;
            }
        }
    }

    // Shield
    if (m_inputService.isKeyPressed(sf::Keyboard::F6)) {
        if (auto* stateManager = player.getStateManager()) {
            try {
                stateManager->applyShield(5.0f);
                std::cout << "[DEBUG] Applied shield" << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception applying shield: " << e.what() << std::endl;
            }
        }
    }

    // DEBUG: Force toggle dark level system with F7
    if (m_inputService.isKeyPressed(sf::Keyboard::F7)) {
        std::cout << "[DEBUG] F7 pressed - Force toggling dark level system" << std::endl;
        if (m_darkLevelSystem) {
            bool wasEnabled = m_darkLevelSystem->isEnabled();
            if (wasEnabled) {
                // Disable dark level
                m_darkLevelSystem->setEnabled(false);
                m_isUnderground = false;
                std::cout << "[DEBUG] Dark level system DISABLED" << std::endl;
            } else {
                // Enable dark level with maximum visibility settings
                m_darkLevelSystem->setEnabled(true);
                m_darkLevelSystem->setDarknessLevel(0.85f); // Slightly less dark for testing
                m_isUnderground = true;
                
                // Clear and add light sources
                m_darkLevelSystem->clearLightSources();
                
                // Add a light source at player position for testing
                auto* transform = getSafeComponent<Transform>(&player);
                if (transform) {
                    sf::Vector2f playerPos = transform->getPosition();
                    m_darkLevelSystem->addLightSource(playerPos, 150.0f, sf::Color(255, 255, 255, 150));
                    std::cout << "[DEBUG] Added test light at player position: (" << playerPos.x << ", " << playerPos.y << ")" << std::endl;
                }
                
                // Add ambient lights
                m_darkLevelSystem->addLightSource(sf::Vector2f(300, 400), 120.0f, sf::Color(255, 200, 100, 100));
                m_darkLevelSystem->addLightSource(sf::Vector2f(800, 300), 80.0f, sf::Color(100, 180, 255, 100));
               
                registerShadowCastingObjects();
            }
        } else {
            std::cout << "[DEBUG] Dark level system is null!" << std::endl;
        }
    }

    // DEBUG: Add F8 key to test flashlight positioning
    if (m_inputService.isKeyPressed(sf::Keyboard::F8)) {
        if (m_darkLevelSystem && m_isUnderground) {
            auto* transform = getSafeComponent<Transform>(&player);
            if (transform) {
                sf::Vector2f playerPos = transform->getPosition();
                
                // Force update player light position
                m_darkLevelSystem->updatePlayerLight(&player);
                
                // Add a temporary bright light at player position
                m_darkLevelSystem->addLightSource(playerPos, 200.0f, sf::Color(255, 255, 255, 200));
                std::cout << "[DEBUG F8] Added bright test light at player position" << std::endl;
            }
        }
    }
}

/**
 * Update camera based on player position
 * @param player Reference to the player entity
 */
void GameplayScreen::updateCameraForPlayer(PlayerEntity& player) {
    m_cameraManager->update(player);
}

/**
 * Update UI elements based on player state
 * @param player Reference to the player entity
 */
void GameplayScreen::updateUI(PlayerEntity& player) {
    try {
        // Get score from player
        int score = player.getScore();

        // Get lives from health component using our safe helper
        int lives = 3; // Default
        auto* health = getSafeComponent<HealthComponent>(&player);
        if (health) {
            lives = health->getHealth();
        }

        // Update UI with player info
        m_ui->update(score, lives);
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception updating UI: " << e.what() << std::endl;
    }
}

/**
 * Set up event handlers for level events
 * Uses observer pattern via event system
 */
void GameplayScreen::setupLevelEventHandlers() {
    // Level transition events
    EventSystem::getInstance().subscribe<LevelTransitionEvent>(
        [this](const LevelTransitionEvent& event) {
            this->onLevelTransition(event);
        }
    );

    // Flag reached events
    EventSystem::getInstance().subscribe<FlagReachedEvent>(
        [this](const FlagReachedEvent& ) {
            this->showLevelCompleteMessage();
        }
    );

    // Well entered events
    EventSystem::getInstance().subscribe<WellEnteredEvent>(
        [this](const WellEnteredEvent& event) {
            this->handleWellEnteredEvent(event);
        }
    );
}

/**
 * Handle well entered events
 * @param event The well entered event
 */
void GameplayScreen::handleWellEnteredEvent(const WellEnteredEvent& event) {
    std::cout << "[GameplayScreen] Well entered event received!" << std::endl;
    std::cout << "[GameplayScreen] Target level: " << event.targetLevel << std::endl;

    // Check for file existence
    std::string levelPath = event.targetLevel;
    if (levelPath.empty()) {
        levelPath = ResourcePaths::DARK_LEVEL;
        std::cout << "[GameplayScreen] Using default dark level" << std::endl;
    }

    // Use our safe level transition method
    if (startLevelTransition(levelPath)) {
        // Check if this is a dark level and activate darkness effects
        activateDarkLevelIfNeeded(levelPath);
    }
}

/**
 * Handle level transition events
 * @param event The level transition event
 */
void GameplayScreen::onLevelTransition(const LevelTransitionEvent& event) {
    // Handle game completion differently than level transitions
    if (event.isGameComplete) {
        showGameCompleteMessage();
    }
    else {
        std::cout << "[GameplayScreen] Level transition event received: " << event.toLevel << std::endl;
        
        // If we're already transitioning, don't start another transition
        if (m_levelTransitionInProgress) {
            std::cout << "[GameplayScreen] Level transition already in progress, ignoring event" << std::endl;
            return;
        }
        
        // Additional safety - check for empty level name
        if (event.toLevel.empty()) {
            std::cerr << "[GameplayScreen] WARNING: Empty level name in transition event" << std::endl;
            return;
        }
        
        // We don't call startLevelTransition here because GameLevelManager already handles the actual loading
        // This event is just informing us about the transition - we just need to prepare our UI state
        
        // Reset game state for UI
        m_showingGameOver = false;
        m_showingLevelComplete = false;
        m_showingGameComplete = false;
        m_messageTimer = 0.0f;
        m_playerValid = false; // Mark player as potentially invalid during transition
    }
}

/**
 * Show level complete message
 */
void GameplayScreen::showLevelCompleteMessage() {
    m_showingLevelComplete = true;
    m_messageTimer = 0.0f;

    // Set level complete text
    std::string levelText = "Level Complete!";
    m_levelCompleteText.setString(levelText);

    // Center the text
    sf::FloatRect bounds = m_levelCompleteText.getLocalBounds();
    m_levelCompleteText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    m_levelCompleteText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

    std::cout << "[GameplayScreen] Showing level complete message" << std::endl;
}

/**
 * Show game complete message
 */
void GameplayScreen::showGameCompleteMessage() {
    m_showingGameComplete = true;
    m_messageTimer = 0.0f;

    // Get player score if available
    PlayerEntity* player = m_gameSession->getPlayer();
    if (player) {
        std::string completeText = std::format("Game Complete!\nFinal Score: {}", player->getScore());
        m_gameCompleteText.setString(completeText);
    }

    // Center the text
    sf::FloatRect bounds = m_gameCompleteText.getLocalBounds();
    m_gameCompleteText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    m_gameCompleteText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

    std::cout << "[GameplayScreen] Showing game complete message" << std::endl;
}

/**
 * Check if player pointer is valid
 * @param player Pointer to the player entity
 * @return true if player is valid, false otherwise
 */
bool GameplayScreen::isPlayerValid(PlayerEntity* player) {
    // Basic null check
    if (!player) {
        return false;
    }
    
    // Check if the player belongs to the current session
    PlayerEntity* currentSessionPlayer = m_gameSession ? m_gameSession->getPlayer() : nullptr;
    if (player != currentSessionPlayer) {
        return false;
    }
    
    // Simple validity check - if we get here, the player is likely valid
    return true;
}

/**
 * Start level transition with proper state reset
 * @param targetLevel The level to transition to
 * @return true if transition was started, false otherwise
 */
bool GameplayScreen::startLevelTransition(const std::string& targetLevel) {
    if (!m_gameSession || m_levelTransitionInProgress) {
        return false;
    }
    
    std::cout << "[GameplayScreen] Starting level transition to: " << targetLevel << std::endl;
    
    m_levelTransitionInProgress = true;
    m_playerValid = false; // Mark player as invalid during transition
    
    // Reset all UI state
    m_showingGameOver = false;
    m_showingLevelComplete = false;
    m_showingGameComplete = false;
    m_messageTimer = 0.0f;
    
    try {
        bool success = m_gameSession->loadLevel(targetLevel);
        m_levelTransitionInProgress = false;
        
        if (success) {
            std::cout << "[GameplayScreen] Level transition complete: " << targetLevel << std::endl;
            
            // IMPORTANT: Activate dark level system if needed
            activateDarkLevelIfNeeded(targetLevel);
            
            // Check if player is valid after transition
            PlayerEntity* newPlayer = m_gameSession->getPlayer();
            m_playerValid = isPlayerValid(newPlayer);
            
            return true;
        }
        else {
            std::cerr << "[GameplayScreen] Level transition failed: " << targetLevel << std::endl;
            return false;
        }
    }
    catch (const std::exception& e) {
        m_levelTransitionInProgress = false;
        std::cerr << "[GameplayScreen] Exception during level transition: " << e.what() << std::endl;
        return false;
    }
}