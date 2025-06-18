#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "IScreen.h"
#include "InputService.h"
#include "Player.h"
#include "Map.h"
#include "ResourceManager.h"
#include "LevelManager.h"
#include "UIOverlay.h"
#include "Constants.h"


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

    std::unique_ptr<Player> m_player;
    std::unique_ptr<Map> m_map;
    std::unique_ptr<UIOverlay> m_ui;

    LevelManager m_levelManager;
};
