#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "IScreen.h"
#include "GameWorld.h"
#include "CameraManager.h"
#include "EnemyManager.h"
#include "ProjectileManager.h"
#include "BackgroundRenderer.h"
#include "InputManager.h"
#include "GameStateManager.h"
#include "UIOverlay.h"
#include "ResourceManager.h"

class GameplayScreen : public IScreen {
public:
    GameplayScreen();
    ~GameplayScreen();

    // IScreen interface
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void initializeComponents();
    void setupGameCallbacks();

    void handleMagneticEffect(float deltaTime, Player& player);

    // Core components
    std::unique_ptr<GameWorld> m_gameWorld;
    std::unique_ptr<CameraManager> m_cameraManager;
    std::unique_ptr<EnemyManager> m_enemyManager;
    std::unique_ptr<ProjectileManager> m_projectileManager;
    std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<GameStateManager> m_gameStateManager;
    std::unique_ptr<UIOverlay> m_ui;
    

    // Resources
    ResourceManager<sf::Texture> m_textures;
    sf::RenderWindow* m_window = nullptr;
};