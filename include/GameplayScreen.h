#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "IScreen.h"
#include "InputService.h"
#include "Ball.h"
#include "Map.h"
#include "ResourceManager.h"
#include "LevelManager.h"

constexpr float WINDOW_WIDTH = 1400.f;
constexpr float WINDOW_HEIGHT = 800.f;


class GameplayScreen : public IScreen {
public:
    GameplayScreen();
    ~GameplayScreen();

    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void loadLevel();
    void updateCamera();

    sf::View m_camera;
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    b2World m_world;
    InputService m_input;
    TextureManager m_textures;

    std::unique_ptr<Ball> m_ball;
    std::unique_ptr<Map> m_map;
    LevelManager m_levelManager;
};
