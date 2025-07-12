#include "GameplayScreen.h"
#include "Constants.h"
#include "PlayerEntity.h"
#include "HealthComponent.h"
#include "Transform.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include "PhysicsComponent.h"
#include <iostream>
#include <format>
#include <WellEntity.h>
#include <typeinfo>

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
    m_isVoiceControlled(false),
    m_showingGameOver(false),
    m_levelTransitionInProgress(false),
    m_playerValid(false) {
    initializeComponents();
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
        m_voiceLevelSystem = std::make_unique<VoiceLevelSystem>();

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

///**
// * Initialize the UI observer with proper error handling
// * Uses lazy initialization pattern - only creates if needed
// * DISABLED - UIObserver usage removed
// */
//void GameplayScreen::initializeUIObserver() {
//    // UIObserver initialization removed
//}

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
        m_gameSession->loadLevel("level1.txt");

        // Initialize UI Observer (DISABLED)
        setupLevelEventHandlers();

        // Initialize dark level system
        m_darkLevelSystem->initialize(window);
        
        // Initialize voice level system
        m_voiceLevelSystem->initialize(window);

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
        m_gameSession->loadLevel("level1.txt");
        std::cout << "[GameplayScreen] Reset to first level" << std::endl;
        break;
    case sf::Keyboard::F2:
        // Load next level
        if (m_gameSession->loadNextLevel()) {
            std::cout << "[GameplayScreen] Manually switched to next level" << std::endl;
        }
        break;
    case sf::Keyboard::F:
        // Toggle flashlight
        if (m_darkLevelSystem && m_isUnderground) {
            m_darkLevelSystem->toggleFlashlight();
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
        
        return;
    }

    // Handle player input and update game state
    try {
        // Handle player input - only if not in voice-controlled level
        if (!m_isVoiceControlled) {
            handlePlayerInput(*player);
        }
        
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
        
        // Update dark level system if in dark level
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
        
        // Update voice level system if in voice-controlled level
        if (m_voiceLevelSystem && m_isVoiceControlled) {
            m_voiceLevelSystem->update(deltaTime, player);
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
    
    // Find all entities with physical bodies that could cast shadows
    std::vector<sf::FloatRect> obstacles;
    
    // Find all ground entities and other obstacles
    if (m_gameSession) {
        for (auto* entity : m_gameSession->getEntityManager().getAllEntities()) {
            if (!entity->isActive()) continue;
            
            // Skip player and projectiles
            if (dynamic_cast<PlayerEntity*>(entity)) continue;
            
            auto* transform = entity->getComponent<Transform>();
            if (transform) {
                sf::Vector2f pos = transform->getPosition();
                sf::Vector2f size(40.0f, 40.0f);  // Default size
                
                // Add to obstacles
                obstacles.emplace_back(pos.x - size.x/2, pos.y - size.y/2, size.x, size.y);
            }
        }
    }
    
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
    // Early exit if no level change is requested or if a level transition is already in progress
    if (!WellEntity::isLevelChangeRequested() || m_levelTransitionInProgress) {
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
    // Check if we should activate the dark level system
    bool shouldActivate = 
        levelName.find("dark") != std::string::npos || 
        levelName.find("underground") != std::string::npos;
        
    // Current level index check (level 2 is dark)
    if (m_gameSession && m_gameSession->getLevelManager().getCurrentLevelIndex() == 1) {
        shouldActivate = true;
    }
    
    if (shouldActivate && m_darkLevelSystem) {
        m_darkLevelSystem->setEnabled(true);
        m_darkLevelSystem->setDarknessLevel(0.92f);
        m_isUnderground = true;

        // Clear any existing light sources
        m_darkLevelSystem->clearLightSources();
        
        // Add ambient light sources for the dark level
        m_darkLevelSystem->addLightSource(sf::Vector2f(300, 400), 120.0f, sf::Color(255, 200, 100, 100)); // Warm torch
        m_darkLevelSystem->addLightSource(sf::Vector2f(800, 300), 80.0f, sf::Color(100, 180, 255, 100)); // Cool blue light

        // Register obstacles for shadow casting
        registerShadowCastingObjects();

        std::cout << "[GameplayScreen] Dark level system activated!" << std::endl;
    } else {
        // Reset dark level settings for normal levels
        if (m_darkLevelSystem) {
            m_darkLevelSystem->setEnabled(false);
            m_isUnderground = false;
        }
    }
}

/**
 * Activate voice level system if needed based on level name
 * @param levelName The name of the loaded level
 */
void GameplayScreen::activateVoiceLevelIfNeeded(const std::string& levelName) {
    // Check if we should activate the voice level system
    bool shouldActivate = 
        levelName.find("voice") != std::string::npos;
        
    // Current level index check (level 3 is voice)
    if (m_gameSession && m_gameSession->getLevelManager().getCurrentLevelIndex() == 2) {
        shouldActivate = true;
    }
    
    if (shouldActivate && m_voiceLevelSystem) {
        m_voiceLevelSystem->setEnabled(true);
        m_isVoiceControlled = true;
        
        // Use NotificationEvent directly instead
        EventSystem::getInstance().publish(
            NotificationEvent("Voice Level: Use V key to simulate voice input", 5.0f)
        );
        
        std::cout << "[GameplayScreen] Voice Level: Use V key to simulate voice input" << std::endl;
    } else {
        // Reset voice level settings for normal levels
        if (m_voiceLevelSystem) {
            m_voiceLevelSystem->setEnabled(false);
            m_isVoiceControlled = false;
        }
    }
}

/**
 * Handle darkness level event from GameLevelManager
 */
void GameplayScreen::onDarknessLevelEvent(const DarknessLevelEvent& event) {
    if (m_darkLevelSystem) {
        m_darkLevelSystem->setEnabled(event.enabled);
        m_darkLevelSystem->setDarknessLevel(event.darknessLevel);
        m_isUnderground = event.enabled;
        
        if (event.enabled) {
            registerShadowCastingObjects();
        }
        
        std::cout << "[GameplayScreen] Darkness level " << (event.enabled ? "enabled" : "disabled") << std::endl;
    }
}

/**
 * Handle voice level event from GameLevelManager
 */
void GameplayScreen::onVoiceLevelEvent(const VoiceLevelEvent& event) {
    if (m_voiceLevelSystem) {
        m_voiceLevelSystem->setEnabled(event.enabled);
        m_isVoiceControlled = event.enabled;
        
        std::cout << "[GameplayScreen] Voice level " << (event.enabled ? "enabled" : "disabled") << std::endl;
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
        // Only handle direct input if not voice-controlled
        if (!m_isVoiceControlled) {
            handlePlayerInput(player);
        }
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

    // Update special level systems as needed
    if (m_darkLevelSystem && m_isUnderground) {
        // Re-get player - it might have changed again
        PlayerEntity* darkLevelPlayer = m_gameSession ? m_gameSession->getPlayer() : nullptr;
        m_darkLevelSystem->update(deltaTime, darkLevelPlayer);
    }
    
    if (m_voiceLevelSystem && m_isVoiceControlled) {
        // Re-get player - it might have changed again
        PlayerEntity* voiceLevelPlayer = m_gameSession ? m_gameSession->getPlayer() : nullptr;
        m_voiceLevelSystem->update(deltaTime, voiceLevelPlayer);
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

        // Position the game over text
        sf::FloatRect bounds = m_gameOverText.getLocalBounds();
        m_gameOverText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        m_gameOverText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 320.0f);

        std::cout << "[GameplayScreen] Game over state activated" << std::endl;
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

    // Render darkness system if in underground level
    if (m_darkLevelSystem && m_isUnderground) {
        // Update player light position before rendering
        PlayerEntity* player = m_gameSession ? m_gameSession->getPlayer() : nullptr;
        if (player) {
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
            }
        }
        
        m_darkLevelSystem->render(window);
    }

    // Switch to UI view for all UI elements
    sf::View defaultView = window.getDefaultView();
    window.setView(defaultView);

    // Render voice level UI if active
    if (m_voiceLevelSystem && m_isVoiceControlled) {
        m_voiceLevelSystem->render(window);
    }

    // Render UI elements
    m_ui->draw(window);

    // Render game over state
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
    // Skip normal input handling if in voice-controlled level
    if (m_isVoiceControlled) return;
    
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

    // Well entered events
    EventSystem::getInstance().subscribe<WellEnteredEvent>(
        [this](const WellEnteredEvent& event) {
            this->handleWellEnteredEvent(event);
        }
    );
    
    // Darkness level events
    EventSystem::getInstance().subscribe<DarknessLevelEvent>(
        [this](const DarknessLevelEvent& event) {
            this->onDarknessLevelEvent(event);
        }
    );
    
    // Voice level events
    EventSystem::getInstance().subscribe<VoiceLevelEvent>(
        [this](const VoiceLevelEvent& event) {
            this->onVoiceLevelEvent(event);
        }
    );
    
    // Notification events
    EventSystem::getInstance().subscribe<NotificationEvent>(
        [](const NotificationEvent& event) {
            std::cout << "[GameplayScreen] Notification: " << event.message << std::endl;
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
        levelPath = "level2.txt";  // Default to next level in sequence
        std::cout << "[GameplayScreen] Using default next level" << std::endl;
    }

    // Use our safe level transition method
    if (startLevelTransition(levelPath)) {
        // Check for special level types
        activateDarkLevelIfNeeded(levelPath);
        activateVoiceLevelIfNeeded(levelPath);
    }
}

/**
 * Handle level transition events
 * @param event The level transition event
 */
void GameplayScreen::onLevelTransition(const LevelTransitionEvent& event) {
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
    m_playerValid = false; // Mark player as potentially invalid during transition
    
    // Check for special level types
    activateDarkLevelIfNeeded(event.toLevel);
    activateVoiceLevelIfNeeded(event.toLevel);
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
    
    try {
        bool success = m_gameSession->loadLevel(targetLevel);
        m_levelTransitionInProgress = false;
        
        if (success) {
            std::cout << "[GameplayScreen] Level transition complete: " << targetLevel << std::endl;
            
            // Check if player is valid after transition
            PlayerEntity* newPlayer = m_gameSession->getPlayer();
            m_playerValid = isPlayerValid(newPlayer);
            
            // Check for special level types
            activateDarkLevelIfNeeded(targetLevel);
            activateVoiceLevelIfNeeded(targetLevel);
            
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