#include "../../include/Commands/EscapeKeyCommand.h"
#include "../../include/Application/AppContext.h"
#include <iostream>

EscapeKeyCommand::EscapeKeyCommand(ScreenType currentScreen, ScreenType targetScreen)
    : m_currentScreen(currentScreen), m_targetScreen(targetScreen) {}

void EscapeKeyCommand::execute() {
    try {
        // Get screen manager
        AppContext::instance().screenManager().requestScreenChange(m_targetScreen);

        // Mark as executed
        m_hasExecuted = true;
    }
    catch (const std::exception& e) {
        std::cout << "EscapeKeyCommand: Failed - " << e.what() << std::endl;
        m_hasExecuted = false;
        throw; // Re-throw for upper level handling
    }
}

void EscapeKeyCommand::undo() {
    if (!m_hasExecuted) {
        return;
    }

    try {
        AppContext::instance().screenManager().requestScreenChange(m_currentScreen);
    }
    catch (const std::exception& e) {
        std::cout << "EscapeKeyCommand: Failed to undo - " << e.what() << std::endl;
    }
}

bool EscapeKeyCommand::canUndo() const {
    return m_hasExecuted && (m_currentScreen != m_targetScreen);
}

std::string EscapeKeyCommand::getName() const {
    return "EscapeKey(" + screenTypeToString(m_currentScreen) +
        " -> " + screenTypeToString(m_targetScreen) + ")";
}

std::string EscapeKeyCommand::screenTypeToString(ScreenType screen) const {
    switch (screen) {
    case ScreenType::LOADING:   return "LOADING";
    case ScreenType::MENU:      return "MENU";
    case ScreenType::PLAY:      return "PLAY";
    case ScreenType::HELP:      return "HELP";
    case ScreenType::SETTINGS:  return "SETTINGS";
    case ScreenType::GAMEOVER:  return "GAMEOVER";
    case ScreenType::WINNING:   return "WINNING";
    case ScreenType::ABOUT_US:  return "ABOUT_US";
    case ScreenType::PAUSE:     return "PAUSE";
    default:                    return "UNKNOWN";
    }
}
