#include "GameplayScreen.h"



GameplayScreen::GameplayScreen()
    : m_world(b2Vec2(0.f, 9.8f)) // Gravity
{
    // Load background
    m_backgroundTexture.loadFromFile("backGroundGame.jpg");
    m_backgroundSprite.setTexture(m_backgroundTexture);
    m_backgroundSprite.setScale(WINDOW_WIDTH / m_backgroundTexture.getSize().x,
        WINDOW_HEIGHT / m_backgroundTexture.getSize().y);

    // Add levels
    m_levelManager.addLevel("level1.txt");
    m_levelManager.addLevel("level2.txt");

    // Initialize map and load first level
    m_map = std::make_unique<Map>(m_world, m_textures);
    loadLevel();

    // Camera setup
    m_camera.setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_camera.setCenter(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
}

GameplayScreen::~GameplayScreen() = default;

void GameplayScreen::loadLevel() {
    const std::string& path = m_levelManager.getCurrentLevelPath();
    m_map->loadFromFile(path);

    // Start ball at top-left of first tile
    m_ball = std::make_unique<Ball>(m_world, 64.f / PPM, 400.f / PPM, m_textures);
}

void GameplayScreen::handleEvents(sf::RenderWindow&) {
    m_input.update();
}

void GameplayScreen::update(float deltaTime) {
    m_world.Step(deltaTime, 8, 3);

    if (m_ball) {
        m_ball->handleInput(m_input);
        m_ball->update(deltaTime);
    }

    updateCamera();
}

void GameplayScreen::render(sf::RenderWindow& window) {
    window.setView(m_camera);
    window.draw(m_backgroundSprite);
    m_map->render(window);
    if (m_ball)
        m_ball->render(window);
}

void GameplayScreen::updateCamera() {
    if (m_ball) {
        sf::Vector2f ballPos = m_ball->getPosition();
        m_camera.setCenter(ballPos.x, WINDOW_HEIGHT / 2.f);
    }
}
