#include "ButtonRenderer.h"
#include <iostream>

//-------------------------------------------------------------------------------------
ButtonRenderer::ButtonRenderer(ButtonModel& model, ButtonInteraction& interaction)
    : m_model(model), m_interaction(interaction) {

    m_background.setFillColor(sf::Color(100, 100, 100, 255));
    m_background.setOutlineColor(sf::Color::White);
    m_background.setOutlineThickness(2);
    m_background.setSize(sf::Vector2f(100, 50));
}
//-------------------------------------------------------------------------------------
void ButtonRenderer::updateTextPosition() {
    if (m_model.texture) {
        return;
    }
    if (!m_model.font) {
        return;
    }

    if (m_model.text.empty()) {
        return;
    }
    // Get background bounds
    sf::FloatRect bounds = m_background.getGlobalBounds();
    if (bounds.width <= 0 || bounds.height <= 0) {
        return;
    }

    sf::Text safeText;
    safeText.setFont(*m_model.font);
    safeText.setString(m_model.text);
    safeText.setCharacterSize(24);
    safeText.setFillColor(m_model.textColor);

    sf::FloatRect textBounds;
    try {
        textBounds = safeText.getLocalBounds();
    }
    catch (...) {
        std::cout << "ButtonRenderer: Exception getting text bounds, skipping" << std::endl;
        return;
    }

    if (textBounds.width <= 0 || textBounds.height <= 0) {
        return;
    }

    // Calculate position
    float centerX = bounds.left + (bounds.width - textBounds.width) / 2.0f;
    float centerY = bounds.top + (bounds.height - textBounds.height) / 2.0f - textBounds.top;

    // Copy the safe text object to member
    m_text = safeText;
    m_text.setPosition(centerX, centerY);

}
//-------------------------------------------------------------------------------------
void ButtonRenderer::updateGraphics() {

    // 1. Always update background
    m_background.setFillColor(m_model.backgroundColor);
    m_background.setSize(m_model.size);
    m_background.setPosition(m_model.position);

    // 2. Update sprite if texture exists
    if (m_model.texture) {
        try {
            m_sprite.setTexture(*m_model.texture);
            m_sprite.setPosition(m_model.position);

            sf::Vector2u textureSize = m_model.texture->getSize();
            if (textureSize.x > 0 && textureSize.y > 0) {
                float scaleX = m_model.size.x / textureSize.x;
                float scaleY = m_model.size.y / textureSize.y;
                m_sprite.setScale(scaleX, scaleY);
            }
        }
        catch (...) {
            std::cout << "ButtonRenderer: Exception updating sprite" << std::endl;
        }
        return;
    }

    // 3. Only update text for NON-textured buttons
    if (m_model.font) {
        updateTextPosition();
    }
}
//-------------------------------------------------------------------------------------
void ButtonRenderer::render(sf::RenderWindow& window) {
    updateGraphics();

    float scale = m_interaction.getHoverScale();
    sf::Vector2f scaledSize = m_model.size * scale;
    sf::Vector2f offset = (scaledSize - m_model.size) * 0.5f;
    sf::Vector2f position = m_model.position - offset;

    // Update background for current frame
    m_background.setSize(scaledSize);
    m_background.setPosition(position);

    // Render hover glow effect
    if (m_interaction.isHovered()) {
        sf::RectangleShape glow;
        glow.setSize(scaledSize + sf::Vector2f(20, 20));
        glow.setPosition(position - sf::Vector2f(10, 10));
        glow.setFillColor(sf::Color(255, 255, 255, 30));
        glow.setOutlineColor(sf::Color(255, 255, 0, 100));
        glow.setOutlineThickness(3);
        window.draw(glow);
    }

    // Render texture OR background (never both)
    if (m_model.texture) {
        // Update sprite scale for hover effect
        sf::Vector2u textureSize = m_model.texture->getSize();
        if (textureSize.x > 0 && textureSize.y > 0) {
            m_sprite.setScale(
                scaledSize.x / textureSize.x,
                scaledSize.y / textureSize.y
            );
        }
        m_sprite.setPosition(position);
        window.draw(m_sprite);

    }
    else {
        window.draw(m_background);

        if (m_model.font) {
            window.draw(m_text);
        }
    }
}
//-------------------------------------------------------------------------------------
