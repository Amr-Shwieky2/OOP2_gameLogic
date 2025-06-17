#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include "IScreen.h"
#include "InputService.h"
#include "Map.h"
#include "Player.h"
#include "UIOverlay.h"
#include "LevelManager.h"
#include "ResourceManager.h"
#include "CollisionResolver.h"
#include "IUpdatable.h"
#include "GameObject.h"

constexpr float WINDOW_WIDTH = 1400.f;
constexpr float WINDOW_HEIGHT = 900.f;

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
    std::vector<std::unique_ptr<IUpdatable>> m_updatables;
};
