#pragma once  
#include "../Core/ICommand.h"  
#include "../Config/ScreenTypes.h"  
#include <string>  

/**
 * @brief Command triggered by Escape key to navigate between screens.
 *
 * Implements the Command Pattern to handle Escape key behavior in a modular way.
 * When executed, it switches from the current screen to a target screen (default: MENU).
 *
 * Supports undo, which reverts the screen back to the original one if needed.
 * Useful for implementing "back" or "exit to menu" functionality.
 */
class EscapeKeyCommand : public ICommand {
public:
    /**
     * @brief Constructs the EscapeKeyCommand.
     *
     * @param currentScreen The screen where the Escape key was pressed.
     * @param targetScreen The screen to navigate to (defaults to MENU).
     */
    EscapeKeyCommand(ScreenType currentScreen, ScreenType targetScreen = ScreenType::MENU);

    /**
     * @brief Executes the command by switching to the target screen.
     */
    void execute() override;

    /**
     * @brief Undoes the command by returning to the original screen.
     */
    void undo() override;

    /**
     * @brief Checks if the command supports undo.
     *
     * @return true if a transition occurred and undo is possible.
     */
    bool canUndo() const override;

    /**
     * @brief Returns a descriptive name for the command.
     *
     * Example: "EscapeKey(SETTINGS -> MENU)"
     */
    std::string getName() const override;

private:
    ScreenType m_currentScreen;    ///< The screen where the command was triggered.
    ScreenType m_targetScreen;     ///< The screen to switch to.
    bool m_hasExecuted = false;    ///< Tracks if the command was successfully executed.

    /**
     * @brief Converts a ScreenType enum to a readable string for logging/debugging.
     */
    std::string screenTypeToString(ScreenType screen) const;
};
