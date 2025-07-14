#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief Handles all UI rendering for Settings Screen
 * Single Responsibility: Visual presentation and animations only
 *
 * This class demonstrates:
 * - Renderer Pattern (separates rendering from logic)
 * - Single Responsibility Principle
 * - Animation system design
 * - Visual effects management
 *
 * Design Patterns Used:
 * - Renderer Pattern
 * - Strategy Pattern (for different visual effects)
 * - Template Method Pattern (for rendering pipeline)
 */
class SettingsUIRenderer {
public:
    /**
     * @brief Constructor with dependency injection
     * @param font Reference to loaded font (injected dependency)
     */
    explicit SettingsUIRenderer(const sf::Font& font);
    ~SettingsUIRenderer() = default;

    // Initialization methods
    void initializeTexts();

    // Animation system - called each frame
    void updateAnimation(float deltaTime);

    // Rendering pipeline - each method has specific purpose
    void renderBackground(sf::RenderWindow& window, const sf::Sprite& background);
    void renderTexts(sf::RenderWindow& window);
    void renderAnimationEffects(sf::RenderWindow& window);

    // Configuration methods for layout flexibility
    void setTitlePosition(float x, float y);
    void setInstructionPosition(float x, float y);

    // Visual effects control
    void setAnimationSpeed(float speed) { m_animationSpeed = speed; }
    void enableGlowEffect(bool enable) { m_glowEnabled = enable; }
    void enableShadowEffect(bool enable) { m_shadowEnabled = enable; }

private:
    const sf::Font& m_font;  // Injected dependency - const reference

    // UI text elements
    sf::Text m_titleText;
    sf::Text m_backInstructionText;

    // Animation state
    float m_animationTime = 0.0f;
    float m_animationSpeed = 1.0f;

    // Visual effect toggles
    bool m_glowEnabled = true;
    bool m_shadowEnabled = true;

    /**
     * @brief Visual configuration structure
     * Encapsulates all visual styling in one place for maintainability
     */
    struct VisualConfig {
        // Desert-themed color palette
        sf::Color titleColor = sf::Color(139, 69, 19);       // Saddle Brown
        sf::Color instructionColor = sf::Color(139, 69, 19); // Matching Brown
        sf::Color shadowColor = sf::Color(50, 33, 17, 120);  // Dark Brown Shadow

        // Typography settings
        int titleSize = 60;
        int instructionSize = 25;

        // Animation parameters
        float glowIntensityMin = 0.7f;
        float glowIntensityMax = 1.0f;
        float glowFrequency = 2.0f;

        // Shadow settings
        sf::Vector2f shadowOffset = sf::Vector2f(3.0f, 3.0f);
    } m_config;

    // Animation helpers - private implementation details
    void updateTitleGlowEffect();
    void createShadowEffect(sf::Text& shadowText, const sf::Text& originalText, sf::Vector2f offset);
    sf::Color calculateGlowColor(float intensity) const;
    float calculateGlowIntensity() const;

    // Utility methods
    void setupTextProperties(sf::Text& text, const std::string& content, int size, sf::Color color);
    void applyTextStyling(sf::Text& text, sf::Text::Style style);
};