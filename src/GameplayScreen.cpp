#include "GameplayScreen.h"
#include "Constants.h"
#include "PlayerEntity.h"
#include "HealthComponent.h"
#include "Transform.h"
#include "UIObserver.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include <iostream>
#include <WellEntity.h>

GameplayScreen::GameplayScreen() {
    initializeComponents();
}

GameplayScreen::~GameplayScreen() = default;

void GameplayScreen::initializeComponents() {
    // Create SRP-compliant GameSession
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

    // Setup UI texts
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

    m_gameOverText.setFont(m_font);
    m_gameOverText.setCharacterSize(32);
    m_gameOverText.setFillColor(sf::Color::White);
    m_gameOverText.setOutlineThickness(2.0f);
    m_gameOverText.setOutlineColor(sf::Color::Black);
    m_gameOverText.setString("Press SPACE to restart");

    // Setup backgrounds
    m_messageBackground.setSize(sf::Vector2f(WINDOW_WIDTH, 200.0f));
    m_messageBackground.setFillColor(sf::Color(0, 0, 0, 180));
    m_messageBackground.setPosition(0, WINDOW_HEIGHT / 2 - 100);

    m_gameOverBackground.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    m_gameOverBackground.setFillColor(sf::Color(0, 0, 0, 150));

    // Try to load Game Over sprite
    try {
        m_gameOverSprite.setTexture(m_textures.getResource("GameOver.png"));
        sf::Vector2u texSize = m_gameOverSprite.getTexture()->getSize();
        m_gameOverSprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
        m_gameOverSprite.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 50.0f);
    }
    catch (const std::exception& e) {
        std::cout << "[WARNING] Could not load GameOver.png: " << e.what() << std::endl;
    }

    std::cout << "[GameplayScreen] Components initialized with SRP GameSession" << std::endl;
}

void GameplayScreen::initializeUIObserver() {
    if (m_font.getInfo().family.empty()) {
        return;
    }

    m_uiObserver = std::make_unique<UIObserver>(m_font);
    m_uiObserver->initialize();

    std::cout << "[GameplayScreen] UI Observer initialized" << std::endl;
}

void GameplayScreen::handleEvents(sf::RenderWindow& window) {
    m_window = &window;

    // One-time initialization
    if (!m_initialized) {
        // Initialize the SRP-compliant GameSession
        m_gameSession->initialize(m_textures, window);

        // Load initial level using the new level manager
        m_gameSession->loadLevel("level1.txt");

        // Initialize UI Observer
        initializeUIObserver();
        setupLevelEventHandlers();

        m_darkLevelSystem->initialize(window);

        m_initialized = true;
        std::cout << "[GameplayScreen] Initialized with SRP GameSession" << std::endl;
    }

    m_inputService.update();

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        m_ui->handleEvent(event, window);

        if (event.type == sf::Event::KeyPressed) {
            // Level switching for testing - now uses SRP methods
            if (event.key.code == sf::Keyboard::F1) {
                // Reset to first level - need to access LevelManager through GameSession
                m_gameSession->loadLevel("level1.txt");
                std::cout << "[GameplayScreen] Reset to first level" << std::endl;
            }
            else if (event.key.code == sf::Keyboard::F2) {
                // Load next level using SRP method
                if (m_gameSession->loadNextLevel()) {
                    std::cout << "[GameplayScreen] Manually switched to next level" << std::endl;
                }
            }
            else if (event.key.code == sf::Keyboard::Space && m_showingGameOver) {
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

    // تحقق من طلبات تغيير المستوى من البئر
    if (WellEntity::isLevelChangeRequested()) {
        std::string targetLevel = WellEntity::getTargetLevelName();
        WellEntity::clearLevelChangeRequest();

        std::cout << "[GameplayScreen] Level change requested: " << targetLevel << std::endl;

        // تحميل المستوى الجديد بأمان
        if (m_gameSession && m_gameSession->loadLevel(targetLevel)) {
            std::cout << "[GameplayScreen] Level loaded successfully: " << targetLevel << std::endl;

            // تفعيل نظام الظلام إذا كان مستوى مظلم
            if (targetLevel.find("dark") != std::string::npos ||
                targetLevel.find("underground") != std::string::npos) {

                if (m_darkLevelSystem) {
                    m_darkLevelSystem->setEnabled(true);
                    m_darkLevelSystem->setDarknessLevel(0.85f);
                    m_isUnderground = true;

                    // إضافة مصادر إضاءة
                    m_darkLevelSystem->addLightSource(sf::Vector2f(300, 400), 80.0f, sf::Color(255, 200, 100));
                    m_darkLevelSystem->addLightSource(sf::Vector2f(800, 300), 60.0f, sf::Color(100, 255, 200));

                    std::cout << "[GameplayScreen] Dark level system activated!" << std::endl;
                }
            }
        }
        else {
            std::cerr << "[GameplayScreen] Failed to load level: " << targetLevel << std::endl;
        }

        return; // اخرج من update loop بعد تحميل المستوى
    }

    // باقي كود update كما هو...

    // Handle message timers
    if (m_showingLevelComplete || m_showingGameComplete) {
        m_messageTimer += deltaTime;
        if (m_messageTimer >= m_messageDuration) {
            m_showingLevelComplete = false;
            m_showingGameComplete = false;
            m_messageTimer = 0.0f;
        }
    }

    // Handle game over
    if (m_showingGameOver) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            std::cout << "[GameplayScreen] Space pressed! Restarting level..." << std::endl;
            m_showingGameOver = false;
            m_gameSession->reloadCurrentLevel();
        }
        return;
    }

    // Get player from the SRP-compliant GameSession
    PlayerEntity* player = m_gameSession->getPlayer();
    if (!player) {
        std::cout << "[WARNING] No player found in GameSession" << std::endl;
        return;
    }

    handlePlayerInput(*player);

    // Update the SRP-compliant GameSession (coordinates all managers)
    m_gameSession->update(deltaTime);

    // Update camera and UI
    PlayerEntity* updatedPlayer = m_gameSession->getPlayer(); // Re-get in case it changed
    if (updatedPlayer && updatedPlayer->hasComponent<Transform>()) {
        updateCameraForPlayer(*updatedPlayer);
        updateUI(*updatedPlayer);
    }

    if (m_uiObserver) {
        m_uiObserver->update(deltaTime);
    }

    if (m_darkLevelSystem) {
        PlayerEntity* updatedPlayer = m_gameSession->getPlayer();
        m_darkLevelSystem->update(deltaTime, updatedPlayer);
    }

    // Check for game over
    auto* health = player ? player->getComponent<HealthComponent>() : nullptr;
    if (health && !health->isAlive() && !m_showingGameOver) {
        m_showingGameOver = true;

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

    // Render game session (uses SRP RenderSystem internally)
    m_gameSession->render(window);

    m_gameSession->render(window);

    if (m_darkLevelSystem && m_isUnderground) {
        m_darkLevelSystem->render(window);
    }

    // Switch to UI view
    sf::View defaultView = window.getDefaultView();
    window.setView(defaultView);

    // Render UI elements
    m_ui->draw(window);

    if (m_uiObserver) {
        m_uiObserver->render(window);
    }

    // Render messages
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

    if (m_showingGameOver) {
        window.draw(m_gameOverBackground);
        window.draw(m_gameOverSprite);
        window.draw(m_gameOverText);
    }
}

void GameplayScreen::handlePlayerInput(PlayerEntity& player) {
    // Player input handling remains the same
    player.handleInput(m_inputService);

    // Debug keys remain the same
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
    m_cameraManager->update(player);
}

void GameplayScreen::updateUI(PlayerEntity& player) {
    int score = player.getScore();

    int lives = 3;
    auto* health = player.getComponent<HealthComponent>();
    if (health) {
        lives = health->getHealth();
    }

    m_ui->update(score, lives);
}

void GameplayScreen::setupLevelEventHandlers() {
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

    EventSystem::getInstance().subscribe<WellEnteredEvent>(
        [this](const WellEnteredEvent& event) {
            std::cout << "[GameplayScreen] Well entered event received!" << std::endl;
            std::cout << "[GameplayScreen] Target level: " << event.targetLevel << std::endl;

            // تأكد من أن GameSession موجود
            if (!m_gameSession) {
                std::cerr << "[GameplayScreen] ERROR: GameSession is null!" << std::endl;
                return;
            }

            // تحقق من وجود الملف
            std::string levelPath = event.targetLevel;
            if (levelPath.empty()) {
                levelPath = "dark_level.txt";
                std::cout << "[GameplayScreen] Using default dark level" << std::endl;
            }

            // تحميل المستوى الجديد
            try {
                std::cout << "[GameplayScreen] Loading underground level: " << levelPath << std::endl;

                if (m_gameSession->loadLevel(levelPath)) {
                    // تفعيل نظام الظلام
                    if (m_darkLevelSystem) {
                        m_darkLevelSystem->setEnabled(true);
                        m_darkLevelSystem->setDarknessLevel(0.85f);
                        m_isUnderground = true;

                        // إضافة مصادر إضاءة
                        m_darkLevelSystem->addLightSource(sf::Vector2f(300, 400), 80.0f, sf::Color(255, 200, 100));
                        m_darkLevelSystem->addLightSource(sf::Vector2f(800, 300), 60.0f, sf::Color(100, 255, 200));

                        std::cout << "[GameplayScreen] Dark level system activated!" << std::endl;
                    }

                    std::cout << "[GameplayScreen] Successfully entered underground level!" << std::endl;
                }
                else {
                    std::cerr << "[GameplayScreen] Failed to load underground level: " << levelPath << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[GameplayScreen] Exception loading underground level: " << e.what() << std::endl;
            }
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

    // Note: We can't easily get level index from SRP GameSession
    // This could be improved by exposing level info through GameSession
    std::string levelText = "Level Complete!";
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