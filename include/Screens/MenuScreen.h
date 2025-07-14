#pragma once
#include "../Core/IScreen.h"
#include "../UI/MenuButtonManager.h"
#include "../UI/MenuAnimationController.h" 
#include "../UI/MenuEventHandler.h"
#include "../UI/MenuRenderer.h"
#include <SFML/Graphics.hpp>
#include <memory>

/**
 * @brief Refactored MenuScreen following Single Responsibility Principle
 * Single Responsibility: Screen lifecycle coordination only
 */
class MenuScreen : public IScreen {
public:
    MenuScreen();
    ~MenuScreen() = default;

    // IScreen interface implementation - delegates to specialized components
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    // Screen lifecycle
    void onEnter() override;
    void onExit() override;

    // Configuration
    void setAnimationSpeed(float speed);
    void enableAnimations(bool enable);

private:
    std::unique_ptr<MenuButtonManager> m_buttonManager;        // Button management
    std::unique_ptr<MenuAnimationController> m_animator;       // Animations
    std::unique_ptr<MenuEventHandler> m_eventHandler;          // Event processing
    std::unique_ptr<MenuRenderer> m_renderer;                  // Rendering

    bool m_isInitialized = false;

    // Initialization
    bool initializeComponents();
    void handleInitializationError(const std::string& component, const std::string& error);

    // Component validation
    bool areComponentsValid() const;
};