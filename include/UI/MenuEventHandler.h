#pragma once
#include <SFML/Graphics.hpp>

class MenuButtonManager;

/**
 * @brief Responsible for menu event processing only
 * Single Responsibility: Event handling and input processing
 */
class MenuEventHandler {
public:
    MenuEventHandler();
    ~MenuEventHandler() = default;

    // Main event processing
    bool handleEvents(sf::RenderWindow& window, MenuButtonManager& buttonManager);

    // Enable/disable event processing
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

private:
    bool m_enabled = true;

    // Specific event handlers
    bool handleWindowEvents(sf::RenderWindow& window, const sf::Event& event);
    bool handleKeyboardEvents(sf::RenderWindow& window, const sf::Event& event);
    bool handleMouseEvents(MenuButtonManager& buttonManager, const sf::Event& event);

    // Input validation
    bool isValidEvent(const sf::Event& event) const;
};