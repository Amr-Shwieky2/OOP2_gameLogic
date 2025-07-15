#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "ButtonModel.h"
#include "ButtonInteraction.h"
#include "ButtonRenderer.h"

/**
 * @brief Button - A reusable GUI button component with MVC design.
 *
 * Combines a visual representation (model + renderer) with interactive behavior
 * (handling mouse input, triggering callbacks). Used in menus and UI systems.
 */
class Button {
public:
    /**
     * @brief Default constructor. Initializes with default size, text, and colors.
     */
    Button();

    /**
     * @brief Parameterized constructor.
     * @param position Top-left position of the button.
     * @param size Width and height of the button.
     * @param text Label text to display on the button.
     */
    Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text);

    // --- Configuration Methods ---

    /// Set button position
    void setPosition(const sf::Vector2f& pos);

    /// Set button size (must be positive)
    void setSize(const sf::Vector2f& size);

    /// Set the text label of the button
    void setText(const std::string& text);

    /// Set the font used for rendering text
    void setFont(const sf::Font& font);

    /// Set optional background texture
    void setTexture(const sf::Texture* texture);

    /// Set background and text colors
    void setColors(const sf::Color& bg, const sf::Color& text);

    /// Set the callback function to execute when the button is clicked
    void setCallback(std::function<void()> callback);

    /// Set only the background image (alias for setTexture)
    void setButtonImage(const sf::Texture* texture);

    /// Set background fill color
    void setBackgroundColor(const sf::Color& color);

    /// Set text color
    void setTextColor(const sf::Color& color);

    // --- Interaction Methods ---

    /// Handle mouse hover and update visual state
    void handleMouseMove(const sf::Vector2f& mousePos);

    /// Handle click and invoke callback if clicked
    bool handleClick(const sf::Vector2f& mousePos);

    /// Update animation or state over time
    void update(float deltaTime);

    /// Render the button to the given window
    void render(sf::RenderWindow& window);

    /// Check if a given mouse position is over the button
    bool isMouseOver(const sf::Vector2f& mousePos) const {
        return m_model.getBounds().contains(mousePos);
    }

private:
    ButtonModel m_model;               ///< Stores button state (position, size, colors, font, etc.)
    ButtonInteraction m_interaction;  ///< Handles input and interaction logic
    ButtonRenderer m_renderer;        ///< Handles visual rendering of the button
};
