#include "../../include/Application/AppContext.h"
#include <iostream>
#include "../../include/Commands/ChangeScreenCommand.h"

/**
 * @brief Constructor - Initialize the command with source and target screens
 * Logs the creation of the command for debugging purposes
 */
ChangeScreenCommand::ChangeScreenCommand(ScreenType targetScreen, ScreenType previousScreen)
    : m_targetScreen(targetScreen), m_previousScreen(previousScreen) {
}

/**
 * @brief Execute the screen change command
 * This method performs the actual screen transition using the AppContext
 */
void ChangeScreenCommand::execute() {
    try {
        // Use AppContext singleton to access screen manager and change screen
        AppContext::instance().screenManager().changeScreen(m_targetScreen);
    }
    catch (const std::exception& e) {
        // Handle any errors that occur during screen change
        std::cout << "ChangeScreenCommand: Failed to change screen - " << e.what() << std::endl;
    }
}

/**
 * @brief Undo the screen change by returning to the previous screen
 * This enables "back" navigation functionality
 */
void ChangeScreenCommand::undo() {
    try {
        // Navigate back to the previous screen
        AppContext::instance().screenManager().changeScreen(m_previousScreen);
    }
    catch (const std::exception& e) {
        // Handle any errors that occur during undo operation
        std::cout << "ChangeScreenCommand: Failed to undo - " << e.what() << std::endl;
    }
}

/**
 * @brief Check if the command can be undone
 * @return true if previous screen is different from target screen
 *
 * Prevents unnecessary undo operations when target and previous screens are the same
 */
bool ChangeScreenCommand::canUndo() const {
    // Can only undo if we're actually changing screens
    return m_previousScreen != m_targetScreen;
}

/**
 * @brief Get a descriptive name for this command
 * @return String describing the screen transition
 *
 * Useful for debugging, logging, or displaying command history
 */
std::string ChangeScreenCommand::getName() const {
    return "ChangeScreen(" + screenTypeToString(m_previousScreen) +
        " -> " + screenTypeToString(m_targetScreen) + ")";
}

/**
 * @brief Convert ScreenType enum to human-readable string
 * @param screen The ScreenType enum value to convert
 * @return String representation of the screen type
 *
 * This helper function makes debugging and logging more readable by
 * converting enum values to descriptive strings
 */
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
        return "UNKNOWN";        // Fallback for unrecognized screen types
    }
}