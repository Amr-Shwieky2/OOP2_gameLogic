#include "GameplayScreen.h"
#include <iostream>
#include "App.h"

GameplayScreen::GameplayScreen()
    : m_world(b2Vec2(0.f, 9.8f)) // Gravity
{
    // Load background
    if (!m_backgroundTexture.loadFromFile("backGroundGame.jpg")) {
        throw std::runtime_error("Failed to load background image.");
    }

    m_backgroundSprite.setTexture(m_backgroundTexture);
    float scaleY = WINDOW_HEIGHT / m_backgroundTexture.getSize().y;
    m_backgroundSprite.setScale(scaleY, scaleY);

    // Load levels
    m_levelManager.addLevel("level1.txt");
    m_levelManager.addLevel("level2.txt");

    // Create map and player
    m_map = std::make_unique<Map>(m_world, m_textures);
    loadLevel();

    m_camera.setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_camera.setCenter(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

    m_ui = std::make_unique<UIOverlay>(WINDOW_WIDTH);
}

GameplayScreen::~GameplayScreen() = default;

void GameplayScreen::loadLevel() {
    const std::string& path = m_levelManager.getCurrentLevelPath();
    m_map->loadFromFile(path);

    m_player = std::make_unique<Player>(m_world, 128.f / PPM, 600.f / PPM, m_textures);
}

void GameplayScreen::handleEvents(sf::RenderWindow& window) {
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

    m_world.Step(deltaTime, 8, 3);

    if (m_player) {
        m_player->handleInput(m_input);
        m_player->update(deltaTime);
        m_ui->update(m_player->getScore(), m_player->getLives());
    }

    if (m_map)
        m_map->update(deltaTime);

    updateCamera();
}

void GameplayScreen::render(sf::RenderWindow& window) {
    window.setView(m_camera);

    float bgWidth = m_backgroundTexture.getSize().x * m_backgroundSprite.getScale().x;
    float camLeft = m_camera.getCenter().x - m_camera.getSize().x / 2.f;
    float camRight = camLeft + m_camera.getSize().x;

    int startTile = static_cast<int>(camLeft / bgWidth) - 1;
    int endTile = static_cast<int>(camRight / bgWidth) + 1;

    for (int i = startTile; i <= endTile; ++i) {
        sf::Sprite repeatedBg = m_backgroundSprite;
        repeatedBg.setPosition(i * bgWidth, 0.f);
        window.draw(repeatedBg);
    }

    if (m_map)
        m_map->render(window);

    if (m_player)
        m_player->render(window);

    m_ui->draw(window);
}

void GameplayScreen::updateCamera() {
    if (!m_player) return;

    sf::Vector2f playerPos = m_player->getPosition();
    float newX = std::max(playerPos.x, WINDOW_WIDTH / 2.f);
    m_camera.setCenter(newX, WINDOW_HEIGHT / 2.f);
}
