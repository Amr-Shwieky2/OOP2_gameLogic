// GameplayScreen.cpp - FULLY INTEGRATED Implementation
#include "GameplayScreen.h"
#include "Constants.h"
#include "PlayerEntity.h"
#include "HealthComponent.h"
#include "Transform.h"
#include "UIObserver.h"  // Include the header file
#include "EventSystem.h"
#include "GameEvents.h"
#include <iostream>

GameplayScreen::GameplayScreen() {
    initializeComponents();
}

GameplayScreen::~GameplayScreen() = default;

void GameplayScreen::initializeComponents() {
    // Initialize core components with new ECS system
    m_gameSession = std::make_unique<GameSession>();
    m_cameraManager = std::make_unique<CameraManager>();
    m_backgroundRenderer = std::make_unique<BackgroundRenderer>(m_textures);
    m_ui = std::make_unique<UIOverlay>(WINDOW_WIDTH);

    // Load font for UI Observer
    if (!m_font.loadFromFile("arial.ttf")) {
        std::cerr << "[WARNING] Failed to load font, UI notifications disabled" << std::endl;
    }

    // Initialize camera
    m_cameraManager->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Initialize game session
    m_gameSession->initialize(m_textures, *m_window);

    std::cout << "[OK] GameplayScreen components initialized with ECS" << std::endl;
}

void GameplayScreen::initializeUIObserver() {
    // Only initialize if font loaded successfully
    if (m_font.getInfo().family.empty()) {
        return;
    }

    // Create and initialize UI Observer
    m_uiObserver = std::make_unique<UIObserver>(m_font);
    m_uiObserver->initialize();

    std::cout << "[OK] UI Observer initialized and listening to events" << std::endl;
}

void GameplayScreen::handleEvents(sf::RenderWindow& window) {
    m_window = &window;

    // One-time initialization
    if (!m_initialized) {
        // Load initial level
        m_gameSession->loadLevel(m_currentLevel);

        // Initialize UI Observer after level is loaded
        initializeUIObserver();

        m_initialized = true;
        std::cout << "[OK] Level loaded: " << m_currentLevel << std::endl;
    }

    // Update input service
    m_inputService.update();

    // Handle window events
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        m_ui->handleEvent(event, window);

        // Handle level switching for testing
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::F1) {
                m_gameSession->loadLevel("level1.txt");
                std::cout << "[OK] Switched to level1.txt" << std::endl;
            }
            else if (event.key.code == sf::Keyboard::F2) {
                m_gameSession->loadLevel("level2.txt");
                std::cout << "[OK] Switched to level2.txt" << std::endl;
            }
            else if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
    }
}

void GameplayScreen::update(float deltaTime) {
    if (m_ui->isPaused()) return;

    // Get player from new system
    PlayerEntity* player = m_gameSession->getPlayer();
    if (!player) {
        std::cout << "[WARNING] No player found in GameSession" << std::endl;
        return;
    }

    // Handle player input with component system
    handlePlayerInput(*player);

    // Update game session (all entities and systems)
    m_gameSession->update(deltaTime);

    // Update camera to follow player
    updateCameraForPlayer(*player);

    // Update UI
    updateUI(*player);

    // Update UI Observer notifications
    if (m_uiObserver) {
        m_uiObserver->update(deltaTime);
    }

    // Check for game over condition
    auto* health = player->getComponent<HealthComponent>();
    if (health && !health->isAlive()) {
        // Publish player died event
        EventSystem::getInstance().publish(
            PlayerDiedEvent(player->getId())
        );

        // Reset level
        std::cout << "[GAME OVER] Player died, restarting level..." << std::endl;
        m_gameSession->loadLevel(m_currentLevel);
    }
}

void GameplayScreen::render(sf::RenderWindow& window) {
    // Set camera view
    m_cameraManager->setView(window);

    // Render background
    m_backgroundRenderer->render(window, m_cameraManager->getCamera());

    // Render game session (all entities)
    m_gameSession->render(window);

    // Render UI (switch to default view)
    sf::View defaultView = window.getDefaultView();
    window.setView(defaultView);

    // Render UI overlay
    m_ui->draw(window);

    // Render UI notifications
    if (m_uiObserver) {
        m_uiObserver->render(window);
    }
}

void GameplayScreen::handlePlayerInput(PlayerEntity& player) {
    // Pass input service to player for handling
    player.handleInput(m_inputService);

    // Additional debug keys
    if (m_inputService.isKeyPressed(sf::Keyboard::F3)) {
        // Debug: Add score
        player.addScore(100);
        std::cout << "[DEBUG] Added 100 score" << std::endl;
    }

    if (m_inputService.isKeyPressed(sf::Keyboard::F4)) {
        // Debug: Damage player
        auto* health = player.getComponent<HealthComponent>();
        if (health) {
            health->takeDamage(1);
            std::cout << "[DEBUG] Player health: " << health->getHealth() << std::endl;
        }
    }

    if (m_inputService.isKeyPressed(sf::Keyboard::F5)) {
        // Debug: Apply speed boost
        player.applySpeedBoost(5.0f);
        std::cout << "[DEBUG] Applied speed boost" << std::endl;
    }

    if (m_inputService.isKeyPressed(sf::Keyboard::F6)) {
        // Debug: Apply shield
        player.applyShield(5.0f);
        std::cout << "[DEBUG] Applied shield" << std::endl;
    }
}

void GameplayScreen::updateCameraForPlayer(PlayerEntity& player) {
    // Use the CameraManager's update method
    m_cameraManager->update(player);
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