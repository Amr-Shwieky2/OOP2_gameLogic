// GameplayScreen.cpp - Fixed Version
#include "GameplayScreen.h"
#include "Constants.h"
#include "CoinEntity.h"  // For magnetic effect
#include <iostream>

GameplayScreen::GameplayScreen() {
    initializeComponents();
}

GameplayScreen::~GameplayScreen() = default;

void GameplayScreen::initializeComponents() {
    // Initialize all components
    m_gameSession = std::make_unique<GameSession>();
    m_cameraManager = std::make_unique<CameraManager>();
    m_projectileManager = std::make_unique<ProjectileManager>();
    m_backgroundRenderer = std::make_unique<BackgroundRenderer>(m_textures);
    m_inputManager = std::make_unique<InputManager>();
    m_ui = std::make_unique<UIOverlay>(WINDOW_WIDTH);

    // Initialize camera
    m_cameraManager->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Initialize game session
    m_gameSession->initialize(m_textures);
    m_gameSession->loadLevel("level1.txt");

}

void GameplayScreen::handleEvents(sf::RenderWindow& window) {
    m_window = &window;

    m_inputManager->handleEvents(window);

    sf::Event event;
    while (window.pollEvent(event)) {
        m_ui->handleEvent(event, window);
    }
}

void GameplayScreen::update(float deltaTime) {
    if (m_ui->isPaused()) return;

    PlayerEntity* player = m_gameSession->getPlayer();
    if (!player) return;

    // Update input and player
    m_inputManager->updatePlayer(*player);

    // Update game session
    m_gameSession->update(deltaTime);

    // Update projectiles
    m_projectileManager->update(deltaTime);

    // Update camera
    m_cameraManager->update(*player);


    // Update UI
    m_ui->update(player->getScore(), player->getLives());

    // Handle magnetic effect
    handleMagneticEffect(deltaTime, *player);
}

void GameplayScreen::render(sf::RenderWindow& window) {
    // Set camera view
    m_cameraManager->setView(window);

    // Render background
    m_backgroundRenderer->render(window, m_cameraManager->getCamera());

    // Render game session
    m_gameSession->render(window);

    // Render enemies
    // Render wind effects (before projectiles for layering)
    PlayerEntity* player = m_gameSession->getPlayer();
    if (player) {
        player->renderWindEffect(window, m_cameraManager->getCamera());
    }

    // Render projectiles
    m_projectileManager->render(window);

    // Render UI
    m_ui->draw(window);
}

void GameplayScreen::setupGameCallbacks() {
    // No additional callbacks required with GameSession
}

void GameplayScreen::handleMagneticEffect(float, PlayerEntity&) {
    // Magnetic effect not implemented in GameSession yet
}