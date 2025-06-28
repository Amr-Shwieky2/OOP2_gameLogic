#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "IScreen.h"
#include "GameSession.h"
#include "CameraManager.h"
#include "ProjectileManager.h"
#include "BackgroundRenderer.h"
#include "InputManager.h"
#include "UIOverlay.h"
#include "ResourceManager.h"
#include "PlayerEntity.h"

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

    void handleMagneticEffect(float deltaTime, PlayerEntity& player);

    // Core components
    std::unique_ptr<GameSession> m_gameSession;
    std::unique_ptr<CameraManager> m_cameraManager;
    std::unique_ptr<ProjectileManager> m_projectileManager;
    std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<UIOverlay> m_ui;
    

    // Resources
    ResourceManager<sf::Texture> m_textures;
    sf::RenderWindow* m_window = nullptr;
};