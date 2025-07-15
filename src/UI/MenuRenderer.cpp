#include "../../include/UI/MenuRenderer.h"
#include "../../include/UI/MenuButtonManager.h"
#include "../../include/UI/MenuAnimationController.h"
#include "../../include/Application/AppContext.h"
#include <iostream>

//-------------------------------------------------------------------------------------
MenuRenderer::MenuRenderer() {
    std::cout << "MenuRenderer: Initialized for rendering only" << std::endl;
}
//-------------------------------------------------------------------------------------
bool MenuRenderer::initializeResources() {
    std::cout << "MenuRenderer: Loading visual resources..." << std::endl;

    bool success = true;

    if (!loadFont()) {
        std::cout << "MenuRenderer: Warning - Using default font" << std::endl;
    }

    if (!loadBackgroundTexture()) {
        std::cout << "MenuRenderer: Creating fallback background..." << std::endl;
        createFallbackBackground();
    }
    else {
        scaleBackgroundToWindow();
    }
    m_resourcesLoaded = true;

    return success;
}
//-------------------------------------------------------------------------------------
bool MenuRenderer::loadFont() {
    try {
        m_font = AppContext::instance().getFont("arial.ttf");
        return true;
    }
    catch (...) {
        return false; 
    }
}
//-------------------------------------------------------------------------------------
bool MenuRenderer::loadBackgroundTexture() {
    try {
        m_backgroundTexture = AppContext::instance().getTexture("MenuScreen.png");
        m_backgroundSprite.setTexture(m_backgroundTexture);
        return true;
    }
    catch (...) {
        return false;
    }
}
//-------------------------------------------------------------------------------------
void MenuRenderer::createFallbackBackground() {
    // Create gradient background if image fails to load
    const int width = 1400, height = 800;
    m_backgroundTexture.create(width, height);

    sf::Uint8* pixels = new sf::Uint8[width * height * 4];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 4;
            float gradient = static_cast<float>(y) / height;
            pixels[index] = static_cast<sf::Uint8>(50 + 100 * gradient);     // R
            pixels[index + 1] = static_cast<sf::Uint8>(30 + 80 * gradient);  // G
            pixels[index + 2] = static_cast<sf::Uint8>(80 + 120 * gradient); // B
            pixels[index + 3] = 255; // A
        }
    }

    m_backgroundTexture.update(pixels);
    m_backgroundSprite.setTexture(m_backgroundTexture);
    delete[] pixels;
}
//-------------------------------------------------------------------------------------
void MenuRenderer::scaleBackgroundToWindow() {
    sf::Vector2u textureSize = m_backgroundTexture.getSize();
    if (textureSize.x > 0 && textureSize.y > 0) {
        sf::Vector2f targetSize(1400.0f, 800.0f);
        float scaleX = targetSize.x / textureSize.x;
        float scaleY = targetSize.y / textureSize.y;
        m_backgroundSprite.setScale(scaleX, scaleY);
    }
}
//-------------------------------------------------------------------------------------
void MenuRenderer::renderBackground(sf::RenderWindow& window) {
    if (hasValidResources()) {
        window.draw(m_backgroundSprite);
    }
}
//-------------------------------------------------------------------------------------
void MenuRenderer::renderButtons(sf::RenderWindow& window, MenuButtonManager& buttonManager) {
    buttonManager.renderButtons(window);
}
//-------------------------------------------------------------------------------------