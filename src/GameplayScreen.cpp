// GameplayScreen.cpp - Fixed Version
#include "GameplayScreen.h"
#include "Constants.h"
#include "Coin.h"  // ← إضافة مطلوبة للـ magnetic effect
#include <iostream>

GameplayScreen::GameplayScreen() {
    initializeComponents();
}

GameplayScreen::~GameplayScreen() = default;

void GameplayScreen::initializeComponents() {
    // Initialize all components
    m_gameWorld = std::make_unique<GameWorld>();
    m_cameraManager = std::make_unique<CameraManager>();
    m_enemyManager = std::make_unique<EnemyManager>();
    m_projectileManager = std::make_unique<ProjectileManager>();
    m_backgroundRenderer = std::make_unique<BackgroundRenderer>(m_textures);
    m_inputManager = std::make_unique<InputManager>();
    m_ui = std::make_unique<UIOverlay>(WINDOW_WIDTH);

    // Initialize camera
    m_cameraManager->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Initialize game world
    m_gameWorld->initialize(m_textures);
}

void GameplayScreen::handleEvents(sf::RenderWindow& window) {
    m_window = &window;

    // Initialize game state manager if not done
    if (!m_gameStateManager) {
        m_gameStateManager = std::make_unique<GameStateManager>(m_textures, window);

        // Load initial level FIRST
        m_gameWorld->loadLevel(m_gameStateManager->getCurrentLevelPath());

        // THEN setup callbacks (now player exists)
        setupGameCallbacks();

        // Setup input for current level
        m_inputManager->initialize(m_gameStateManager->getCurrentLevelPath() == "level2.txt" ? 1 : 0);
    }

    m_inputManager->handleEvents(window);

    sf::Event event;
    while (window.pollEvent(event)) {
        m_ui->handleEvent(event, window);
    }
}

void GameplayScreen::update(float deltaTime) {
    if (m_ui->isPaused()) return;

    Player* player = m_gameWorld->getPlayer();
    if (!player) return;

    // Update input and player
    m_inputManager->updatePlayer(*player);

    // Update game world
    m_gameWorld->update(deltaTime);

    // Update enemies
    if (Map* map = m_gameWorld->getMap()) {
        m_enemyManager->update(deltaTime, *player, map->getObjects());
    }

    // Update projectiles
    m_projectileManager->update(deltaTime);

    // Update camera
    m_cameraManager->update(*player);

    // Update game state
    if (m_gameStateManager) {  // ← إضافة null check
        m_gameStateManager->update(deltaTime, *player);
    }

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

    // Render game world
    m_gameWorld->render(window);

    // Render projectiles
    m_projectileManager->render(window);

    // Render UI
    m_ui->draw(window);
}

void GameplayScreen::setupGameCallbacks() {
    if (m_gameStateManager && m_gameWorld) {
        m_gameStateManager->setSpawnCallback(
            [this](std::unique_ptr<GameObject> obj) {
                m_gameWorld->spawnGameObject(std::move(obj));
            }
        );

        Player* player = m_gameWorld->getPlayer();
        if (player) {
            std::cout << "✅ Player found and set successfully!" << std::endl;
            m_gameStateManager->setPlayer(player);
            m_gameStateManager->initialize();
        }
        else {
            std::cout << "❌ Warning: Player is still null after loadLevel!" << std::endl;
        }
    }
}

void GameplayScreen::handleMagneticEffect(float deltaTime, Player& player) {
    if (!player.hasEffect(PlayerEffect::Magnetic)) return;

    Map* map = m_gameWorld->getMap();
    if (!map) return;

    sf::Vector2f playerPos = player.getPosition();

    for (auto& obj : map->getObjects()) {
        if (auto* coin = dynamic_cast<Coin*>(obj.get())) {
            if (!coin->isCollected()) {
                sf::Vector2f coinPos = coin->getPosition();
                float dist = std::hypot(playerPos.x - coinPos.x, playerPos.y - coinPos.y);

                if (dist < 350.f) {
                    float speed = 300.f * deltaTime;
                    coin->moveTowards(playerPos, speed);
                }
            }
        }
    }
}