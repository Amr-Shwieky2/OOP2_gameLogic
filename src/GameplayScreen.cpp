#include "GameplayScreen.h"
#include "SurpriseBoxScreen.h"
#include <iostream>
#include "App.h"

GameplayScreen::GameplayScreen()
    : m_world(b2Vec2(0.f, 9.8f))
{
    if (!m_backgroundTexture.loadFromFile("backGroundGame.jpeg")) {
        throw std::runtime_error("Failed to load background image.");
    }

    m_backgroundSprite.setTexture(m_backgroundTexture);
    float scaleY = WINDOW_HEIGHT / m_backgroundTexture.getSize().y;
    m_backgroundSprite.setScale(scaleY, scaleY);

    m_levelManager.addLevel("level1.txt");
    m_levelManager.addLevel("level2.txt");

    m_map = std::make_unique<Map>(m_world, m_textures);
    loadLevel();

    m_camera.setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_camera.setCenter(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

    m_ui = std::make_unique<UIOverlay>(WINDOW_WIDTH);

    if (m_levelManager.getCurrentIndex() == 1) {
		m_world.SetGravity(b2Vec2(0.f, 18.0f)); // Disable gravity for level 2
        m_voiceInput.start();
    }

    std::cout << "GameplayScreen initialized with collision and surprise box systems!" << std::endl;
}

GameplayScreen::~GameplayScreen() {
    m_voiceInput.stop();
}

void GameplayScreen::loadLevel() {
    const std::string& path = m_levelManager.getCurrentLevelPath();
    m_map->loadFromFile(path);

    m_player = std::make_unique<Player>(m_world, 128.f / PPM, 600.f / PPM, m_textures);

    m_collisionSystem = std::make_unique<CollisionSystem>(
        *m_player,
        [this](std::unique_ptr<GameObject> obj) { spawnGameObject(std::move(obj)); }
    );

    std::cout << "Level loaded with collision and surprise box systems!" << std::endl;
}

void GameplayScreen::handleEvents(sf::RenderWindow& window) {
    m_window = &window;

    if (!m_surpriseBoxManager && m_window) {
        m_surpriseBoxManager = std::make_unique<SurpriseBoxManager>(m_textures, *m_window);
        m_surpriseBoxManager->setSpawnCallback(
            [this](std::unique_ptr<GameObject> obj) { spawnGameObject(std::move(obj)); }
        );
        std::cout << "SurpriseBoxManager created after window assignment!" << std::endl;
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

    m_world.Step(deltaTime, 8, 3);

    if (m_player) {
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
        m_player->update(deltaTime);
        m_ui->update(m_player->getScore(), m_player->getLives());
    }

    if (m_map) {
        m_map->update(deltaTime);
        if (m_collisionSystem) {
            m_collisionSystem->checkCollisions(m_map->getObjects());
        }
    }

    if (m_surpriseBoxManager) {
        static int lastScore = 0;
        int currentScore = m_player->getScore();
        if (currentScore > lastScore && (currentScore % 10) == 0) {
            m_surpriseBoxManager->onCoinCollected();
        }
        lastScore = currentScore;
    }

    updateCamera();

    static float debugTimer = 0.0f;
    debugTimer += deltaTime;
    if (debugTimer >= 2.0f) {
        std::cout << "Player - Score: " << m_player->getScore()
            << ", Lives: " << m_player->getLives() << std::endl;
        debugTimer = 0.0f;
    }
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

void GameplayScreen::spawnGameObject(std::unique_ptr<GameObject> obj) {
    if (!obj) return;

    std::cout << "Spawning new game object from box!" << std::endl;

    if (auto dynamicObj = dynamic_cast<DynamicGameObject*>(obj.get())) {
        auto ptr = std::unique_ptr<DynamicGameObject>(static_cast<DynamicGameObject*>(obj.release()));
        m_map->addDynamic(std::move(ptr));
    }
    else {
        m_map->addStatic(std::move(obj));
    }
}
