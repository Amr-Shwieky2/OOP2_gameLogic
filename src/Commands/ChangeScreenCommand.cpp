#include "../../include/Application/AppContext.h"
#include <iostream>
#include "../../include/Commands/ChangeScreenCommand.h"

//-------------------------------------------------------------------------------------
ChangeScreenCommand::ChangeScreenCommand(ScreenType targetScreen, ScreenType previousScreen)
    : m_targetScreen(targetScreen), m_previousScreen(previousScreen) {
}
//-------------------------------------------------------------------------------------
void ChangeScreenCommand::execute() {
    try {
        AppContext::instance().screenManager().requestScreenChange(m_targetScreen);
    }
    catch (const std::exception& e) {
        std::cout << "ChangeScreenCommand: Failed to change screen - " << e.what() << std::endl;
    }
}
//-------------------------------------------------------------------------------------
void ChangeScreenCommand::undo() {
    try {
        AppContext::instance().screenManager().requestScreenChange(m_previousScreen);
    }
    catch (const std::exception& e) {
        std::cout << "ChangeScreenCommand: Failed to undo - " << e.what() << std::endl;
    }
}
//-------------------------------------------------------------------------------------
bool ChangeScreenCommand::canUndo() const {
    // Can only undo if we're actually changing screens
    return m_previousScreen != m_targetScreen;
}
//-------------------------------------------------------------------------------------
std::string ChangeScreenCommand::getName() const {
    return "ChangeScreen(" + screenTypeToString(m_previousScreen) +
        " -> " + screenTypeToString(m_targetScreen) + ")";
}
//-------------------------------------------------------------------------------------
std::string ChangeScreenCommand::screenTypeToString(ScreenType screen) const {
    switch (screen) {
    case ScreenType::MENU:
        return "MENU";           // Main menu screen
    case ScreenType::PLAY:
        return "PLAY";           // Gameplay screen
    case ScreenType::SETTINGS:
        return "SETTINGS";       // Settings/options screen
    case ScreenType::HELP:
        return "HELP";           // Help/tutorial screen
    case ScreenType::ABOUT_US:
        return "ABOUT_US";       // About/credits screen
    case ScreenType::PAUSE:
        return "PAUSE";          // Pause menu screen
    case ScreenType::GAMEOVER:
        return "GAME_OVER";      // Game over screen
    default:
        return "UNKNOWN";       
    }
}
//-------------------------------------------------------------------------------------