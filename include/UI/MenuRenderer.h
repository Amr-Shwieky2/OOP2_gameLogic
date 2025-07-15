#pragma once
#include <SFML/Graphics.hpp>

class MenuButtonManager;
class MenuAnimationController;

/**
 * @brief Responsible for menu rendering only
 * Single Responsibility: Visual presentation and drawing
 */
class MenuRenderer {
public:
    MenuRenderer();
    ~MenuRenderer() = default;

    // Initialization
    bool initializeResources();

    // Rendering pipeline
    void renderBackground(sf::RenderWindow& window);
    void renderButtons(sf::RenderWindow& window, MenuButtonManager& buttonManager);

    // Resource management
    bool hasValidResources() const { return m_resourcesLoaded; }

private:
    // Visual resources
    sf::Font m_font;
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
    bool m_resourcesLoaded = false;

    // Resource loading
    bool loadFont();
    bool loadBackgroundTexture();
    void createFallbackBackground();

    // Rendering helpers
    void scaleBackgroundToWindow();
};
