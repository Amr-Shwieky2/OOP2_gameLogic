#include "../../include/UI/SettingsUIRenderer.h"
#include <iostream>
#include <cmath>

//-------------------------------------------------------------------------------------
SettingsUIRenderer::SettingsUIRenderer(const sf::Font& font) : m_font(font) {}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::initializeTexts() {
    // Setup title text with hardcoded English
    setupTextProperties(m_titleText,
        "Settings", 
        m_config.titleSize,
        m_config.titleColor);
    m_titleText.setPosition(570, 70);
    applyTextStyling(m_titleText, sf::Text::Italic);

    // Setup instruction text  
    setupTextProperties(m_backInstructionText,
        "Press ESC to go back",  
        m_config.instructionSize,
        m_config.instructionColor);
    m_backInstructionText.setPosition(570, 550);
    applyTextStyling(m_backInstructionText, sf::Text::Italic);
}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::updateAnimation(float deltaTime) {
    // Update animation timer with configurable speed
    m_animationTime += deltaTime * m_animationSpeed;

    // Update individual animation effects
    if (m_glowEnabled) {
        updateTitleGlowEffect();
    }
}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::updateTitleGlowEffect() {
    float glowIntensity = calculateGlowIntensity();
    sf::Color glowColor = calculateGlowColor(glowIntensity);
    m_titleText.setFillColor(glowColor);
}
//-------------------------------------------------------------------------------------
float SettingsUIRenderer::calculateGlowIntensity() const {
    // Use sine wave to create smooth oscillation
    float sineValue = std::sin(m_animationTime * m_config.glowFrequency);

    float normalizedSine = (sineValue + 1.0f) * 0.5f; 
    return m_config.glowIntensityMin +
        (normalizedSine * (m_config.glowIntensityMax - m_config.glowIntensityMin));
}
//-------------------------------------------------------------------------------------
sf::Color SettingsUIRenderer::calculateGlowColor(float intensity) const {
    sf::Color baseColor = m_config.titleColor;
    baseColor.a = static_cast<sf::Uint8>(255 * intensity);
    return baseColor;
}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::renderBackground(sf::RenderWindow& window, const sf::Sprite& background) {
    window.draw(background);
}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::renderTexts(sf::RenderWindow& window) {
    // Render title with shadow effect
    if (m_shadowEnabled) {
        sf::Text titleShadow = m_titleText;
        createShadowEffect(titleShadow, m_titleText, m_config.shadowOffset);
        window.draw(titleShadow);
    }
    // Render main title text
    window.draw(m_titleText);

    // Render instruction text (no shadow needed for smaller text)
    window.draw(m_backInstructionText);
}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::createShadowEffect(sf::Text& shadowText, const sf::Text& originalText, sf::Vector2f offset) {
    // Configure shadow appearance
    shadowText.setFillColor(m_config.shadowColor);

    // Position shadow with offset
    sf::Vector2f originalPos = originalText.getPosition();
    shadowText.setPosition(originalPos.x + offset.x, originalPos.y + offset.y);
}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::renderAnimationEffects(sf::RenderWindow& window) {}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::setTitlePosition(float x, float y) {
    m_titleText.setPosition(x, y);
}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::setInstructionPosition(float x, float y) {
    m_backInstructionText.setPosition(x, y);
}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::setupTextProperties(sf::Text& text, const std::string& content, int size, sf::Color color) {
    text.setFont(m_font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);
}
//-------------------------------------------------------------------------------------
void SettingsUIRenderer::applyTextStyling(sf::Text& text, sf::Text::Style style) {
    text.setStyle(style);
}
//-------------------------------------------------------------------------------------