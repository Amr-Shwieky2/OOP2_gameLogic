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
#include <RenderComponent.h>

GameplayScreen::GameplayScreen() {
    initializeComponents();
}

GameplayScreen::~GameplayScreen() = default;

void GameplayScreen::initializeComponents() {
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

    m_levelCompleteText.setFont(m_font);
    m_levelCompleteText.setCharacterSize(48);
    m_levelCompleteText.setFillColor(sf::Color::Yellow);
    m_levelCompleteText.setOutlineThickness(3.0f);
    m_levelCompleteText.setOutlineColor(sf::Color::Black);
    m_levelCompleteText.setString("Level Complete!");

    m_gameCompleteText.setFont(m_font);
    m_gameCompleteText.setCharacterSize(54);
    m_gameCompleteText.setFillColor(sf::Color::Yellow);
    m_gameCompleteText.setOutlineThickness(3.0f);
    m_gameCompleteText.setOutlineColor(sf::Color::Black);
    m_gameCompleteText.setString("Game Complete! Congratulations!");

    m_messageBackground.setSize(sf::Vector2f(WINDOW_WIDTH, 200.0f));
    m_messageBackground.setFillColor(sf::Color(0, 0, 0, 180));
    m_messageBackground.setPosition(0, WINDOW_HEIGHT / 2 - 100);

    std::cout << "[OK] GameplayScreen components initialized with ECS" << std::endl;
    // إعداد Game Over
    m_gameOverText.setFont(m_font);
    m_gameOverText.setCharacterSize(32);
    m_gameOverText.setFillColor(sf::Color::White);
    m_gameOverText.setOutlineThickness(2.0f);
    m_gameOverText.setOutlineColor(sf::Color::Black);
    m_gameOverText.setString("Press SPACE to restart");

    // إعداد خلفية Game Over
    m_gameOverBackground.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    m_gameOverBackground.setFillColor(sf::Color(0, 0, 0, 150)); // شفاف أسود

    // محاولة تحميل صورة Game Over
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
        // Initialize game session with window NOW
        m_gameSession->initialize(m_textures, window);

        // Load initial level 
        m_gameSession->loadLevel(m_gameSession->getLevelManager().getCurrentLevelPath());

        // Initialize UI Observer after level is loaded
        initializeUIObserver();

        setupLevelEventHandlers();

        m_initialized = true;
        std::cout << "[OK] Level loaded with level management" << std::endl;
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
                m_gameSession->getLevelManager().resetToFirstLevel();
                m_gameSession->loadLevel(m_gameSession->getLevelManager().getCurrentLevelPath());
                std::cout << "[OK] Reset to first level" << std::endl;
            }
            else if (event.key.code == sf::Keyboard::F2) {
                if (m_gameSession->loadNextLevel()) {
                    std::cout << "[OK] Manually switched to next level" << std::endl;
                }
            }
            else if (event.key.code == sf::Keyboard::Space && m_showingGameOver) {
                // إعادة تشغيل المستوى عند الضغط على SPACE
                std::cout << "[GameplayScreen] Restarting level after Game Over..." << std::endl;
                m_showingGameOver = false;
                m_gameSession->reloadCurrentLevel();
            }

            else if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
    }
}
void GameplayScreen::update(float deltaTime) {
    if (m_ui->isPaused()) return;

    if (m_showingLevelComplete || m_showingGameComplete) {
        m_messageTimer += deltaTime;
        if (m_messageTimer >= m_messageDuration) {
            m_showingLevelComplete = false;
            m_showingGameComplete = false;
            m_messageTimer = 0.0f;
        }
    }

    if (m_showingGameOver) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            std::cout << "[GameplayScreen] Space pressed! Restarting level..." << std::endl;
            m_showingGameOver = false;
            m_gameSession->reloadCurrentLevel();
        }
        return; 
    }

    PlayerEntity* player = m_gameSession->getPlayer();
    if (!player) {
        std::cout << "[WARNING] No player found in GameSession" << std::endl;
        return;
    }

    handlePlayerInput(*player);
    m_gameSession->update(deltaTime);

    player = m_gameSession->getPlayer();
    if (player && player->hasComponent<Transform>()) {
        updateCameraForPlayer(*player);
        updateUI(*player);
    }

    if (m_uiObserver) {
        m_uiObserver->update(deltaTime);
    }

    auto* health = player ? player->getComponent<HealthComponent>() : nullptr;
    if (health && !health->isAlive() && !m_showingGameOver) {
        // إظهار شاشة Game Over
        m_showingGameOver = true;

        // "Press SPACE to restart"
        sf::FloatRect bounds = m_gameOverText.getLocalBounds();
        m_gameOverText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        m_gameOverText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 320.0f);

        EventSystem::getInstance().publish(PlayerDiedEvent(player->getId()));
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

    if (m_showingLevelComplete) {
        window.draw(m_messageBackground);

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

    // Game Over
    if (m_showingGameOver) {
        window.draw(m_gameOverBackground);
        window.draw(m_gameOverSprite);
        window.draw(m_gameOverText);
    }
}

void GameplayScreen::handlePlayerInput(PlayerEntity& player) {
    // Use the player's input handler
    player.handleInput(m_inputService);

    // Debug keys that affect subsystems directly
    if (m_inputService.isKeyPressed(sf::Keyboard::F3)) {
        if (auto* scoreManager = player.getScoreManager()) {
            scoreManager->addScore(100);
            std::cout << "[DEBUG] Added 100 score" << std::endl;
        }
    }

    if (m_inputService.isKeyPressed(sf::Keyboard::F4)) {
        auto* health = player.getComponent<HealthComponent>();
        if (health) {
            health->takeDamage(1);
            std::cout << "[DEBUG] Player health: " << health->getHealth() << std::endl;
        }
    }

    if (m_inputService.isKeyPressed(sf::Keyboard::F5)) {
        if (auto* stateManager = player.getStateManager()) {
            stateManager->applySpeedBoost(5.0f);
            std::cout << "[DEBUG] Applied speed boost" << std::endl;
        }
    }

    if (m_inputService.isKeyPressed(sf::Keyboard::F6)) {
        if (auto* stateManager = player.getStateManager()) {
            stateManager->applyShield(5.0f);
            std::cout << "[DEBUG] Applied shield" << std::endl;
        }
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

void GameplayScreen::setupLevelEventHandlers() {
    // الاشتراك في أحداث انتقال المستوى
    EventSystem::getInstance().subscribe<LevelTransitionEvent>(
        [this](const LevelTransitionEvent& event) {
            this->onLevelTransition(event);
        }
    );

    EventSystem::getInstance().subscribe<FlagReachedEvent>(
        [this](const FlagReachedEvent& event) {
            this->showLevelCompleteMessage();
        }
    );
}

void GameplayScreen::onLevelTransition(const LevelTransitionEvent& event) {
    if (event.isGameComplete) {
        showGameCompleteMessage();
    }
    else {
        std::cout << "[GameplayScreen] Transitioning to: " << event.toLevel << std::endl;
    }
}

void GameplayScreen::showLevelCompleteMessage() {
    m_showingLevelComplete = true;
    m_messageTimer = 0.0f;

    std::string levelText = "Level " + std::to_string(m_gameSession->getLevelManager().getCurrentIndex() + 1) + " Complete!";
    m_levelCompleteText.setString(levelText);

    sf::FloatRect bounds = m_levelCompleteText.getLocalBounds();
    m_levelCompleteText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    m_levelCompleteText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

    std::cout << "[GameplayScreen] Showing level complete message" << std::endl;
}

void GameplayScreen::showGameCompleteMessage() {
    m_showingGameComplete = true;
    m_messageTimer = 0.0f;

    PlayerEntity* player = m_gameSession->getPlayer();
    if (player) {
        std::string completeText = "Game Complete!\nFinal Score: " + std::to_string(player->getScore());
        m_gameCompleteText.setString(completeText);
    }

    sf::FloatRect bounds = m_gameCompleteText.getLocalBounds();
    m_gameCompleteText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    m_gameCompleteText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

    std::cout << "[GameplayScreen] Showing game complete message" << std::endl;
}