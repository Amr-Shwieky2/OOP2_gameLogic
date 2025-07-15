#include "../../include/core/SettingsResourceManager.h"
#include <Application/AppContext.h>
#include <iostream>

//-------------------------------------------------------------------------------------
SettingsResourceManager::SettingsResourceManager() : m_initialized(false) {}
//-------------------------------------------------------------------------------------
bool SettingsResourceManager::initializeResources() {
    try {
        // Acquire resources from the shared managers
        m_font = &AppContext::instance().getFont(FONT_PATH);

        try {
            m_backgroundTexture = &AppContext::instance().getTexture(BACKGROUND_PATH);
            m_backgroundSprite.setTexture(*m_backgroundTexture);
            scaleBackgroundToWindow();
        }
        catch (const std::exception&) {
            std::cout << "External background not found, generating procedural background..." << std::endl;
            setupFallbackBackground();
        }

        m_initialized = true;
        std::cout << "Settings resources initialized successfully using shared managers" << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cout << "Exception during resource initialization: " << e.what() << std::endl;
        m_initialized = false;
        return false;
    }
}
//-------------------------------------------------------------------------------------
void SettingsResourceManager::setupFallbackBackground() {
    // Create texture programmatically
    m_fallbackTexture.create(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Allocate pixel data - using RAII-like cleanup pattern
    sf::Uint8* pixels = new sf::Uint8[WINDOW_WIDTH * WINDOW_HEIGHT * 4];

    try {
        // Generate desert color gradient
        for (unsigned int y = 0; y < WINDOW_HEIGHT; ++y) {
            for (unsigned int x = 0; x < WINDOW_WIDTH; ++x) {
                unsigned int index = (y * WINDOW_WIDTH + x) * 4;

                // Calculate gradient factor (0.0 to 1.0)
                float gradient = static_cast<float>(y) / WINDOW_HEIGHT;

                pixels[index] = static_cast<sf::Uint8>(50 + 100 * gradient);     
                pixels[index + 1] = static_cast<sf::Uint8>(30 + 80 * gradient);  
                pixels[index + 2] = static_cast<sf::Uint8>(80 + 120 * gradient); 
                pixels[index + 3] = 255; 
            }
        }

        m_fallbackTexture.update(pixels);
        m_backgroundTexture = &m_fallbackTexture;
        m_backgroundSprite.setTexture(m_fallbackTexture);
    }
    catch (const std::exception& e) {
        std::cout << "Error generating procedural background: " << e.what() << std::endl;
    }

    // Clean up allocated memory - important for preventing memory leaks
    delete[] pixels;
}
//-------------------------------------------------------------------------------------
void SettingsResourceManager::scaleBackgroundToWindow() {
    if (!m_backgroundTexture)
        return;

    sf::Vector2u textureSize = m_backgroundTexture->getSize();

    if (textureSize.x > 0 && textureSize.y > 0) {
        sf::Vector2f targetSize(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
        float scaleX = targetSize.x / textureSize.x;
        float scaleY = targetSize.y / textureSize.y;
        m_backgroundSprite.setScale(scaleX, scaleY);
    }
    else {
        std::cout << "Warning: Invalid texture dimensions for scaling" << std::endl;
    }
}
//-------------------------------------------------------------------------------------