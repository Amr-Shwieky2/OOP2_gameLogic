#include "GameplayScreen.h"

GameplayScreen::GameplayScreen()
    : m_world(b2Vec2(0.f, 9.8f)) // Gravity
{
    // Load background texture
    if (!m_backgroundTexture.loadFromFile("backGroundGame.jpg")) {
        throw std::runtime_error("Failed to load background image.");
    }

    m_backgroundSprite.setTexture(m_backgroundTexture);

    // Scale background to match window height
    float scaleY = WINDOW_HEIGHT / m_backgroundTexture.getSize().y;
    m_backgroundSprite.setScale(scaleY, scaleY);

    // Add level files
    m_levelManager.addLevel("level1.txt");
    m_levelManager.addLevel("level2.txt");

    // Load map
    m_map = std::make_unique<Map>(m_world, m_textures);

    // Load first level
    loadLevel();

    // Initialize camera
    m_camera.setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_camera.setCenter(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

    // Initialize UI
    m_ui = std::make_unique<UIOverlay>(WINDOW_WIDTH);
}

GameplayScreen::~GameplayScreen() = default;

void GameplayScreen::loadLevel() {
    const std::string& path = m_levelManager.getCurrentLevelPath();
    m_map->loadFromFile(path);

    // Create the player
    m_player = std::make_unique<Player>(m_world, 128.f / PPM, 600.f / PPM, m_textures);
}

void GameplayScreen::handleEvents(sf::RenderWindow& window) {
    m_input.update();

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        // Pass event to UI (e.g., pause button)
        m_ui->handleEvent(event, window);
    }
}

void GameplayScreen::update(float deltaTime) {
    // Skip updating game logic if paused
    if (m_ui->isPaused()) return;

    m_world.Step(deltaTime, 8, 3);

    if (m_player) {
        m_player->handleInput(m_input);
        m_player->update(deltaTime);

        // Update UI with live values from player
        m_ui->update(m_player->getScore(), m_player->getLives());
    }

    updateCamera();
}

void GameplayScreen::render(sf::RenderWindow& window) {
    window.setView(m_camera);

    // Repeat background based on camera position
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

    m_map->render(window);

    if (m_player)
        m_player->render(window);

    // Draw UI (score, lives, pause, timer)
    m_ui->draw(window);
}

void GameplayScreen::updateCamera() {
    if (m_player) {
        sf::Vector2f playerPos = m_player->getPosition();
        float newX = std::max(playerPos.x, WINDOW_WIDTH / 2.f);
        m_camera.setCenter(newX, WINDOW_HEIGHT / 2.f);
    }
}
