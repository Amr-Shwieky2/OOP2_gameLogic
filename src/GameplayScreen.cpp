// GameplayScreen.cpp - Cleaned Version (بدون الطباعات والتعليقات غير المهمة)

#include "GameplayScreen.h"
#include "SurpriseBoxScreen.h"
#include "MultiMethodCollisionSystem.h"
#include <iostream>
#include "App.h"

GameplayScreen::GameplayScreen() {
    initializeWorld();
    initializeGameState();
    initializeBackground();
    initializeLevels();
    initializePlayer();
    initializeSystems();
    initializeUI();
}

GameplayScreen::~GameplayScreen() {
    m_voiceInput.stop();
}

void GameplayScreen::initializeWorld() {
    m_world = std::make_unique<b2World>(b2Vec2(0.f, 9.8f));
}

void GameplayScreen::initializeGameState() {
    m_gameState = std::make_unique<GameState>();
}

void GameplayScreen::initializeBackground() {
    if (!m_backgroundTexture.loadFromFile("backGroundGame.jpeg")) {
        throw std::runtime_error("Failed to load background image.");
    }

    m_backgroundSprite.setTexture(m_backgroundTexture);
    float scaleY = WINDOW_HEIGHT / m_backgroundTexture.getSize().y;
    m_backgroundSprite.setScale(scaleY, scaleY);
}

void GameplayScreen::initializeLevels() {
    m_levelManager.addLevel("level1.txt");
    m_levelManager.addLevel("level2.txt");

    m_map = std::make_unique<Map>(*m_world, m_textures);
    loadCurrentLevel();
}

void GameplayScreen::initializePlayer() {
    m_player = std::make_unique<Player>(*m_world, 128.f / PPM, 600.f / PPM, m_textures);
}

void GameplayScreen::initializeSystems() {
    m_camera = std::make_unique<CameraController>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_multiMethodCollisions = std::make_unique<MultiMethodCollisionSystem>(*m_player, *m_gameState);
    updateLevelSettings();
}

void GameplayScreen::initializeUI() {
    m_ui = std::make_unique<UIOverlay>(WINDOW_WIDTH);
}

void GameplayScreen::handleEvents(sf::RenderWindow& window) {
    m_window = &window;

    if (!m_surpriseBoxManager && m_window) {
        m_surpriseBoxManager = std::make_unique<SurpriseBoxManager>(m_textures, *m_window);
        m_surpriseBoxManager->setSpawnCallback(
            [this](std::unique_ptr<GameObject> obj) { spawnGameObject(std::move(obj)); }
        );
    }

    m_input.update();

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        m_ui->handleEvent(event, window);
    }
}

void GameplayScreen::update(float deltaTime) {
    if (m_ui->isPaused()) return;

    if (m_gameState) {
        m_gameState->updateTime(deltaTime);
    }

    handleMenuInput();
    updateInput(deltaTime);
    updatePhysics(deltaTime);
    updateGameLogic(deltaTime);
    updateSystems(deltaTime);
    checkGameEvents();
}

void GameplayScreen::handleMenuInput() {
    if (m_gameState->isLevelComplete()) {
        if (m_input.isKeyPressed(sf::Keyboard::Space)) {
            proceedToNextLevel();
        }
    }

    if (m_gameState->isGameOver()) {
        if (m_input.isKeyPressed(sf::Keyboard::R)) {
            restartGame();
        }
    }
}

void GameplayScreen::updateInput(float deltaTime) {
    if (!m_player) return;
    if (m_gameState->isGameOver()) return;

    if (m_levelManager.getCurrentIndex() == 1) {
        float volume = m_voiceInput.getVolume();
        if (volume > 0.7f) {
            m_player->jump();
        }
        else if (volume > 0.2f) {
            m_player->moveForward(volume);
        }
    }
    else {
        m_player->handleInput(m_input);
    }
}

void GameplayScreen::updatePhysics(float deltaTime) {
    m_world->Step(deltaTime, 8, 3);
}

void GameplayScreen::updateGameLogic(float deltaTime) {
    if (m_player) m_player->update(deltaTime);
    if (m_map) m_map->update(deltaTime);
}

void GameplayScreen::updateSystems(float deltaTime) {
    if (m_player && m_camera) {
        sf::Vector2f playerPos = m_player->getPosition();
        sf::Vector2f playerVel = m_player->getVelocity();
        m_camera->followTarget(playerPos, playerVel);
        m_camera->update(deltaTime);
    }

    if (m_multiMethodCollisions && m_map) {
        m_multiMethodCollisions->checkAllCollisions(m_map->getObjects());
    }

    if (m_surpriseBoxManager) {
        static int lastScore = 0;
        int currentScore = m_gameState->getScore();
        if (currentScore > lastScore && (currentScore % 10) == 0) {
            m_surpriseBoxManager->onCoinCollected();
        }
        lastScore = currentScore;
    }

    if (m_ui && m_gameState) {
        m_ui->update(*m_gameState);
    }
}

void GameplayScreen::checkGameEvents() {
    if (m_gameState->getLives() <= 0 && !m_gameState->isGameOver()) {
        handleGameOver();
    }

    if (m_gameState->getScore() >= 1000 && !m_gameState->isLevelComplete()) {
        handleLevelComplete();
    }
}

void GameplayScreen::render(sf::RenderWindow& window) {
    if (m_camera) {
        window.setView(m_camera->getView());
    }

    renderBackground(window);
    renderGameWorld(window);
    renderUI(window);
}

void GameplayScreen::renderBackground(sf::RenderWindow& window) {
    if (!m_camera) return;

    float bgWidth = m_backgroundTexture.getSize().x * m_backgroundSprite.getScale().x;
    float camLeft = m_camera->getCenter().x - m_camera->getSize().x / 2.f;
    float camRight = camLeft + m_camera->getSize().x;

    int startTile = static_cast<int>(camLeft / bgWidth) - 1;
    int endTile = static_cast<int>(camRight / bgWidth) + 1;

    for (int i = startTile; i <= endTile; ++i) {
        sf::Sprite repeatedBg = m_backgroundSprite;
        repeatedBg.setPosition(i * bgWidth, 0.f);
        window.draw(repeatedBg);
    }
}

void GameplayScreen::renderGameWorld(sf::RenderWindow& window) {
    if (m_map) m_map->render(window);
    if (m_player) m_player->render(window);
}

void GameplayScreen::renderUI(sf::RenderWindow& window) {
    if (m_ui && m_gameState) m_ui->draw(window, *m_gameState);
}

void GameplayScreen::loadCurrentLevel() {
    const std::string& path = m_levelManager.getCurrentLevelPath();
    m_map->loadFromFile(path);
}

void GameplayScreen::spawnGameObject(std::unique_ptr<GameObject> obj) {
    if (!obj || !m_map) return;

    if (auto dynamicObj = dynamic_cast<DynamicGameObject*>(obj.get())) {
        auto ptr = std::unique_ptr<DynamicGameObject>(static_cast<DynamicGameObject*>(obj.release()));
        m_map->addDynamic(std::move(ptr));
    }
    else {
        m_map->addStatic(std::move(obj));
    }
}

void GameplayScreen::onCoinCollected() {}

void GameplayScreen::handleLevelComplete() {
    if (!m_gameState->isLevelComplete()) {
        m_gameState->setLevelComplete(true);
    }
}

void GameplayScreen::handleGameOver() {
    if (!m_gameState->isGameOver()) {
        m_gameState->setGameOver(true);
    }
}

void GameplayScreen::proceedToNextLevel() {
    if (m_levelManager.loadNextLevel()) {
        m_gameState->resetLevel();
        loadCurrentLevel();
        resetPlayerPosition();
        updateLevelSettings();
    }
    else {
        handleGameComplete();
    }
}

void GameplayScreen::resetPlayerPosition() {
    if (m_player && m_world) {
        m_player.reset();
        m_player = std::make_unique<Player>(*m_world, 128.f / PPM, 600.f / PPM, m_textures);

        if (m_multiMethodCollisions) {
            m_multiMethodCollisions.reset();
            m_multiMethodCollisions = std::make_unique<MultiMethodCollisionSystem>(*m_player, *m_gameState);
        }

        if (m_camera) {
            m_camera->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
        }
    }
}

void GameplayScreen::updateLevelSettings() {
    int currentLevel = m_levelManager.getCurrentIndex();

    if (currentLevel == 1) {
        m_world->SetGravity(b2Vec2(0.f, 18.0f));
        m_voiceInput.start();
    }
    else {
        m_world->SetGravity(b2Vec2(0.f, 9.8f));
        m_voiceInput.stop();
    }
}

void GameplayScreen::handleGameComplete() {
    restartGame();
}

void GameplayScreen::restartGame() {
    m_gameState->resetGame();
    m_levelManager.loadLevel(0);
    loadCurrentLevel();
    resetPlayerPosition();
    updateLevelSettings();
}
