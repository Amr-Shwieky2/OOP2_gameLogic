#pragma once
#include "Button.h"
#include "../Core/ButtonSubject.h"
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

/**
 * @brief Enhanced button with Observer Pattern support
 *
 * Inherits from regular Button class and adds notification capabilities.
 * When clicked or hovered, it automatically notifies all registered observers.
 * This creates a clean separation between UI events and game logic.
 */
class ObservableButton : public Button, public ButtonSubject {
public:
    /**
     * @brief Constructor - creates button with unique ID
     * @param position Button position on screen
     * @param size Button dimensions
     * @param text Display text
     * @param id Unique identifier for this button
     */
    ObservableButton(const sf::Vector2f& position, const sf::Vector2f& size,
        const std::string& text, const std::string& id)
        : Button(position, size, text), m_id(id), m_wasHovered(false) {
        std::cout << "Created ObservableButton: " << m_id << std::endl;
    }

    /**
     * @brief Get button's unique identifier
     * @return Button ID string
     */
    const std::string& getId() const {
        return m_id;
    }

    /**
     * @brief Handle mouse click with Observer Pattern
     *
     * Calls parent's click handling for visual effects,
     * then notifies all observers about the click event.
     *
     * @param mousePos Current mouse position
     * @return true if button was clicked, false otherwise
     */
    bool handleClick(const sf::Vector2f& mousePos)  {
        if (Button::handleClick(mousePos)) {
            std::cout << "Button '" << m_id << "' clicked!" << std::endl;

            // 🎯 Observer Pattern in action!
            // Notify all registered observers about this click
            notifyClicked(m_id);
            return true;
        }
        return false;
    }

    /**
     * @brief Handle mouse movement with hover detection
     *
     * Updates visual hover effects through parent class,
     * then sends hover notifications to observers when
     * mouse enters button area (not every frame).
     *
     * @param mousePos Current mouse position
     */
    void handleMouseMove(const sf::Vector2f& mousePos)  {
        // Call parent method for visual hover effects
        Button::handleMouseMove(mousePos);

        // Check if mouse is currently over button
        bool currentlyHovered = isMouseOver(mousePos);

        // Send notification only when hover state changes
        // (when mouse enters button area, not every frame)
        if (currentlyHovered && !m_wasHovered) {
            std::cout << "Button '" << m_id << "' hovered!" << std::endl;

            // Observer Pattern in action!
            // Notify observers about hover event
            notifyHovered(m_id);
        }

        // Remember current hover state for next frame
        m_wasHovered = currentlyHovered;
    }

private:
    std::string m_id;        // Unique button identifier
    bool m_wasHovered;       // Track hover state to avoid spam notifications
};