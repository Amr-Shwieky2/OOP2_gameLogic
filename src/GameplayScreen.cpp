// ==========================================
// GameplayScreen.cpp - Updated Implementation
// ==========================================

#include "GameplayScreen.h"
#include "Constants.h"
#include "PlayerEntity.h"
#include <iostream>
#include <HealthComponent.h>
#include <Transform.h>

GameplayScreen::GameplayScreen() {
    initializeComponents();
}

GameplayScreen::~GameplayScreen() = default;

void GameplayScreen::initializeComponents() {
    // Initialize core components - CHANGED: GameSession instead of GameWorld
    m_gameSession = std::make_unique<GameSession>();
    m_cameraManager = std::make_unique<CameraManager>();
    m_backgroundRenderer = std::make_unique<BackgroundRenderer>(m_textures);
    m_inputManager = std::make_unique<InputManager>();
    m_ui = std::make_unique<UIOverlay>(WINDOW_WIDTH);

    // Initialize camera
    m_cameraManager->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Initialize game session
    m_gameSession->initialize(m_textures);

    std::cout << "GameplayScreen components initialized" << std::endl;
}

void GameplayScreen::handleEvents(sf::RenderWindow& window) {
    m_window = &window;

    // One-time initialization
    if (!m_initialized) {
        // Load initial level
        m_gameSession->loadLevel(m_currentLevel);

        // Setup input system
        m_inputManager->initialize(0);  // Level 0 = keyboard input

        m_initialized = true;
        std::cout << "GameplayScreen initialized with level: " << m_currentLevel << std::endl;
    }

    // Handle window events
    m_inputManager->handleEvents(window);

    sf::Event event;
    while (window.pollEvent(event)) {
        m_ui->handleEvent(event, window);

        // Handle level switching for testing
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::F1) {
                m_gameSession->loadLevel("level1.txt");
                std::cout << "Switched to level1.txt" << std::endl;
            }
            else if (event.key.code == sf::Keyboard::F2) {
                m_gameSession->loadLevel("level2.txt");
                std::cout << "Switched to level2.txt" << std::endl;
            }
        }
    }
}

void GameplayScreen::update(float deltaTime) {
    if (m_ui->isPaused()) return;

    // Get player from new system
    PlayerEntity* player = m_gameSession->getPlayer();
    if (!player) {
        std::cout << "Warning: No player found in GameSession" << std::endl;
        return;
    }

    // Handle player input
    handlePlayerInput(*player);

    // Update game session (replaces GameWorld, EnemyManager, ProjectileManager updates)
    m_gameSession->update(deltaTime);

    // Update camera to follow player
    updateCameraForPlayer(*player);

    // Update UI
    updateUI(*player);
}

void GameplayScreen::render(sf::RenderWindow& window) {
    // Set camera view
    m_cameraManager->setView(window);

    // Render background
    m_backgroundRenderer->render(window, m_cameraManager->getCamera());

    // Render game session (replaces GameWorld, EnemyManager, ProjectileManager rendering)
    m_gameSession->render(window);

    // Render UI (switch to default view)
    sf::View defaultView = window.getDefaultView();
    window.setView(defaultView);
    m_ui->draw(window);
}

void GameplayScreen::handlePlayerInput(PlayerEntity& player) {
    // This will need to be adapted once PlayerEntity has proper input handling
    // For now, we'll use the InputService approach

    // TODO: Implement proper input handling for PlayerEntity
    // This might involve adding an InputComponent or StateComponent

    // Temporary: Direct input handling
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player.moveLeft();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player.moveRight();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        player.jump();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
        player.shoot();
    }
}

void GameplayScreen::updateCameraForPlayer(PlayerEntity& player) {
    // Get player position from Transform component
    auto* transform = player.getComponent<Transform>();
    if (transform) {
        sf::Vector2f playerPos = transform->getPosition();

        // Update camera position (simplified - you may want to smooth this)
        float newX = std::max(playerPos.x, WINDOW_WIDTH / 2.f);
        sf::Vector2f cameraCenter(newX, WINDOW_HEIGHT / 2.f);

        // Update camera (you'll need to add this method to CameraManager)
        // m_cameraManager->setCenterPosition(cameraCenter);

        // For now, use the existing update method if it exists
        // This might need adjustment based on your CameraManager implementation
    }
}

void GameplayScreen::updateUI(PlayerEntity& player) {
    // Get score and lives from player
    int score = player.getScore();

    // Get health from HealthComponent
    int lives = 3;  // Default
    auto* health = player.getComponent<HealthComponent>();
    if (health) {
        lives = health->getHealth();
    }

    m_ui->update(score, lives);
}

void GameplayScreen::setupGameCallbacks() {
    // This method can be removed since GameSession handles entity management internally
    // Or it can be used for setting up observers/events in the future
    std::cout << "Game callbacks setup (placeholder for future event system)" << std::endl;
}