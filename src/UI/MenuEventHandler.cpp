#include "../../include/UI/MenuEventHandler.h"
#include "../../include/UI/MenuButtonManager.h"
#include <iostream>

//-------------------------------------------------------------------------------------
MenuEventHandler::MenuEventHandler() {
}
//-------------------------------------------------------------------------------------
bool MenuEventHandler::handleEvents(sf::RenderWindow& window, MenuButtonManager& buttonManager) {
    if (!m_enabled) {
        return false;
    }

    sf::Event event;
    while (window.pollEvent(event)) {
        if (!isValidEvent(event)) {
            continue;
        }

        // Handle different event types
        if (handleWindowEvents(window, event)) {
            return true; // Window should close
        }

        if (handleKeyboardEvents(window, event)) {
            return true; // Exit requested
        }

        handleMouseEvents(buttonManager, event);
    }

    return false; // Continue running
}
//-------------------------------------------------------------------------------------
bool MenuEventHandler::handleWindowEvents(sf::RenderWindow& window, const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        std::cout << "MenuEventHandler: Window close requested" << std::endl;
        window.close();
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------------
bool MenuEventHandler::handleKeyboardEvents(sf::RenderWindow& window, const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        std::cout << "MenuEventHandler: Escape key pressed - closing window" << std::endl;
        window.close();
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------------
bool MenuEventHandler::handleMouseEvents(MenuButtonManager& buttonManager, const sf::Event& event) {
    bool handled = false;

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(
            static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y)
        );
        buttonManager.handleButtonHover(mousePos);
        handled = true;
    }
    else if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y)
        );
        handled = buttonManager.handleButtonClick(mousePos);
    }

    return handled;
}
//-------------------------------------------------------------------------------------
bool MenuEventHandler::isValidEvent(const sf::Event& event) const {
    return true;
}
//-------------------------------------------------------------------------------------