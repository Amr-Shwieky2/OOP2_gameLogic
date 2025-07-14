#include "../Settings/SettingsInputHandler.h"
#include <iostream>

SettingsInputHandler::SettingsInputHandler() {
    std::cout << "SettingsInputHandler: Initialized for input processing only" << std::endl;
}

bool SettingsInputHandler::handleKeyboardInput(const sf::Event& event) {
    // Early return if input handling is disabled
    if (!m_enabled) {
        return false;
    }

    // Validate input event
    if (!isValidKeyboardEvent(event)) {
        return false;
    }

    // Handle different key types
    try {
        switch (event.key.code) {
        case sf::Keyboard::Escape:
            return handleEscapeKey();

        case sf::Keyboard::U:
            if (isModifierKeyPressed(event)) {
                return handleUndoKey();
            }
            break;

        case sf::Keyboard::R:
            if (isModifierKeyPressed(event)) {
                return handleRedoKey();
            }
            break;

        case sf::Keyboard::H:
            if (isModifierKeyPressed(event)) {
                return handleHistoryKey();
            }
            break;

        default:
            return false; // Key not handled
        }
    }
    catch (const std::exception& e) {
        std::cout << "SettingsInputHandler: Error handling key: " << e.what() << std::endl;
        return false;
    }

    return false;
}

bool SettingsInputHandler::handleEscapeKey() {
    if (m_escapeCallback) {
        std::cout << "SettingsInputHandler: Processing Escape key" << std::endl;
        m_escapeCallback();
        return true;
    }
    return false;
}

bool SettingsInputHandler::handleUndoKey() {
    if (m_undoCallback) {
        std::cout << "SettingsInputHandler: Processing Undo key (Ctrl+U)" << std::endl;
        m_undoCallback();
        return true;
    }
    return false;
}

bool SettingsInputHandler::handleRedoKey() {
    if (m_redoCallback) {
        std::cout << "SettingsInputHandler: Processing Redo key (Ctrl+R)" << std::endl;
        m_redoCallback();
        return true;
    }
    return false;
}

bool SettingsInputHandler::handleHistoryKey() {
    if (m_historyCallback) {
        std::cout << "SettingsInputHandler: Processing History key (Ctrl+H)" << std::endl;
        m_historyCallback();
        return true;
    }
    return false;
}

bool SettingsInputHandler::isValidKeyboardEvent(const sf::Event& event) const {
    return event.type == sf::Event::KeyPressed;
}

bool SettingsInputHandler::isModifierKeyPressed(const sf::Event& event) const {
    return event.key.control || event.key.alt || event.key.shift;
}

bool SettingsInputHandler::isKeyPressed(const sf::Event& event, sf::Keyboard::Key key) const {
    return isValidKeyboardEvent(event) && event.key.code == key;
}