#include "../../include/core/SettingsResourceManager.h"
#include <iostream>

//-------------------------------------------------------------------------------------
SettingsResourceManager::SettingsResourceManager() : m_initialized(false) {}
//-------------------------------------------------------------------------------------
bool SettingsResourceManager::initializeResources() {
    try {
        // Load font first - most critical resource for UI
        if (!loadFont()) {
            std::cout << "Warning: Using default SFML font for Settings UI" << std::endl;
            // Note: We continue even if external font fails (graceful degradation)
        }

        // Load background texture with fallback strategy
        if (!loadBackgroundTexture()) {
            std::cout << "External background not found, generating procedural background..." << std::endl;
            setupFallbackBackground();
        }
        else {
            scaleBackgroundToWindow();
        }

        m_initialized = true;
        std::cout << "Settings resources initialized successfully using RAII principles" << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cout << "Exception during resource initialization: " << e.what() << std::endl;
        m_initialized = false;
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool SettingsResourceManager::loadFont() {
    try {
        if (m_font.loadFromFile(FONT_PATH)) {
            return true;
        }
        return false;

    }
    catch (const std::exception& e) {
        std::cout << "Exception loading font: " << e.what() << std::endl;
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool SettingsResourceManager::loadBackgroundTexture() {
    try {
        bool loaded = m_backgroundTexture.loadFromFile(BACKGROUND_PATH);
        if (loaded) {
            m_backgroundSprite.setTexture(m_backgroundTexture);
        }
        return loaded;

    }
    catch (const std::exception& e) {
        std::cout << "Exception loading background texture: " << e.what() << std::endl;
        return false;
    }
}
//-------------------------------------------------------------------------------------
void SettingsResourceManager::setupFallbackBackground() {
    // Create texture programmatically
    m_backgroundTexture.create(WINDOW_WIDTH, WINDOW_HEIGHT);

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

        m_backgroundTexture.update(pixels);
        m_backgroundSprite.setTexture(m_backgroundTexture);
    }
    catch (const std::exception& e) {
        std::cout << "Error generating procedural background: " << e.what() << std::endl;
    }

    // Clean up allocated memory - important for preventing memory leaks
    delete[] pixels;
}
//-------------------------------------------------------------------------------------
void SettingsResourceManager::scaleBackgroundToWindow() {
    sf::Vector2u textureSize = m_backgroundTexture.getSize();

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