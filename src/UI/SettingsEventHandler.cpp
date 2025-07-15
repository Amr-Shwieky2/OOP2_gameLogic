#include "SettingsEventHandler.h"
#include "VolumeControlPanel.h"
#include "../../include/Commands/ChangeScreenCommand.h"
#include <Application/AppContext.h>
#include <iostream>

//-------------------------------------------------------------------------------------
SettingsEventHandler::SettingsEventHandler() {}
//-------------------------------------------------------------------------------------
SettingsEventHandler::~SettingsEventHandler() {
    cleanup();
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::cleanup() {
    try {
        if (m_volumePanel) {
            m_volumePanel.reset();
        }
    }
    catch (const std::exception& e) {
        std::cout << "Error during cleanup: " << e.what() << std::endl;
    }
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::setVolumePanel(std::shared_ptr<VolumeControlPanel> panel) {
    if (panel) {
        m_volumePanel = panel;
    }
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::handleEvents(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        try {
            handleWindowEvents(window, event);
            handleKeyboardEvents(event);
            handleMouseEvents(event);
        }
        catch (const std::exception& e) {
            std::cout << "Error handling event: " << e.what() << std::endl;
            window.close();
        }
    }

    // Detect Escape polling (just in case)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        std::cout << "Escape detected via polling - requesting exit to menu" << std::endl;
        processEscapeKey();
    }
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::handleWindowEvents(sf::RenderWindow& window, const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        std::cout << "Window close event received" << std::endl;
        window.close();
    }
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::handleKeyboardEvents(const sf::Event& event) {
    if (!isValidKeyboardEvent(event)) return;

    if (event.type == sf::Event::KeyPressed) {
        std::cout << "Key pressed detected: " << event.key.code << std::endl;
        handleKeyPressEvents(event);
    }
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::handleKeyPressEvents(const sf::Event& event) {
    std::cout << "Processing key code: " << event.key.code << std::endl;

    switch (event.key.code) {
    case sf::Keyboard::Escape:
        processEscapeKey();
        break;

    default:
        std::cout << "Unhandled key: " << event.key.code << std::endl;
        break;
    }
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::handleMouseEvents(const sf::Event& event) {
    if (!isValidMouseEvent(event)) return;

    switch (event.type) {
    case sf::Event::MouseMoved:
        handleMouseMoveEvents(event);
        break;

    case sf::Event::MouseButtonPressed:
        handleMousePressEvents(event);
        break;

    case sf::Event::MouseButtonReleased:
        handleMouseReleaseEvents(event);
        break;

    default:
        break;
    }
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::handleMouseMoveEvents(const sf::Event& event) {
    delegateMouseEventToComponents(event);
}

void SettingsEventHandler::handleMousePressEvents(const sf::Event& event) {
    if (event.mouseButton.button == sf::Mouse::Left) {
        if (delegateMouseEventToComponents(event)) {
            std::cout << "Mouse press handled by component" << std::endl;
        }
    }
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::handleMouseReleaseEvents(const sf::Event& event) {
    if (event.mouseButton.button == sf::Mouse::Left) {
        delegateMouseEventToComponents(event);
    }
}
//-------------------------------------------------------------------------------------
bool SettingsEventHandler::delegateMouseEventToComponents(const sf::Event& event) {
    try {
        if (m_volumePanel) {
            return m_volumePanel->handleMouseEvent(event);
        }
    }
    catch (const std::exception& e) {
        std::cout << "Error delegating mouse event: " << e.what() << std::endl;
        cleanup();
    }

    return false;
}
//-------------------------------------------------------------------------------------
void SettingsEventHandler::processEscapeKey() {
    std::cout << "Escape key pressed - issuing ChangeScreenCommand to MENU" << std::endl;
    auto command = std::make_unique<ChangeScreenCommand>(ScreenType::MENU, ScreenType::SETTINGS);
    AppContext::instance().commandInvoker().execute(std::move(command));
}
//-------------------------------------------------------------------------------------
bool SettingsEventHandler::isValidMouseEvent(const sf::Event& event) const {
    switch (event.type) {
    case sf::Event::MouseMoved:
    case sf::Event::MouseButtonPressed:
    case sf::Event::MouseButtonReleased:
        return true;
    default:
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool SettingsEventHandler::isValidKeyboardEvent(const sf::Event& event) const {
    switch (event.type) {
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
        return true;
    default:
        return false;
    }
}
//-------------------------------------------------------------------------------------