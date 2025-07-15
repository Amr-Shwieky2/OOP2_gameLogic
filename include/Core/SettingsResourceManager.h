#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief Handles resource loading for Settings Screen
 * Single Responsibility: Resource management only
 *
 * This class demonstrates the Resource Manager pattern and
 * separation of concerns principle in OOP design.
 *
 * Design Patterns Used:
 * - Resource Manager Pattern
 * - RAII (Resource Acquisition Is Initialization)
 * - Exception Safety
 */
class SettingsResourceManager {
public:
    SettingsResourceManager();
    ~SettingsResourceManager() = default;

    // Resource initialization with comprehensive error handling
    bool initializeResources();

    // Resource accessors with const correctness
    const sf::Font& getFont() const { return m_font; }
    const sf::Texture& getBackgroundTexture() const { return m_backgroundTexture; }
    const sf::Sprite& getBackgroundSprite() const { return m_backgroundSprite; }

    // Resource validation
    bool isInitialized() const { return m_initialized; }

private:
    // Resource objects - managed automatically by RAII
    sf::Font m_font;
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
    bool m_initialized = false;

    // Individual resource loaders - single purpose methods
    bool loadFont();
    bool loadBackgroundTexture();
    void setupFallbackBackground();
    void scaleBackgroundToWindow();

    // Constants for maintainability
    static constexpr unsigned int WINDOW_WIDTH = 1400;
    static constexpr unsigned int WINDOW_HEIGHT = 800;
    static constexpr const char* FONT_PATH = "arial.ttf";
    static constexpr const char* BACKGROUND_PATH = "SettingsScreen.png";
};