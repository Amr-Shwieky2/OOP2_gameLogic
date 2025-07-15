#pragma once
#include "../Core/ICommand.h"
#include "../Config/ScreenTypes.h"
#include <string>

/**
 * @brief Command object for changing the active screen in the application.
 *
 * Implements the Command Pattern, allowing encapsulation of screen transitions.
 * Supports undo functionality by storing both the target screen and the previous screen.
 *
 * Used by UI buttons, navigation logic, or state machines to decouple screen switching logic.
 */
class ChangeScreenCommand : public ICommand {
public:
    /**
     * @brief Constructs a ChangeScreenCommand with the desired target and previous screens.
     *
     * @param targetScreen The screen to switch to when the command is executed.
     * @param previousScreen The screen to return to when the command is undone.
     */
    ChangeScreenCommand(ScreenType targetScreen, ScreenType previousScreen);

    /**
     * @brief Executes the command, initiating a transition to the target screen.
     */
    void execute() override;

    /**
     * @brief Undoes the command, returning to the previous screen.
     */
    void undo() override;

    /**
     * @brief Checks whether the command supports undo.
     *
     * @return true if target and previous screens are different, false otherwise.
     */
    bool canUndo() const override;

    /**
     * @brief Returns a descriptive name for the command (e.g., "ChangeScreen(MENU -> PLAY)").
     *
     * Useful for debugging, logging, or command history tracking.
     */
    std::string getName() const override;

private:
    ScreenType m_targetScreen;     ///< The screen to switch to when the command is executed.
    ScreenType m_previousScreen;   ///< The screen to return to when the command is undone.

    /**
     * @brief Converts a ScreenType enum value into a human-readable string.
     *
     * @param screen The screen type to convert.
     * @return A string representation of the screen (e.g., "MENU", "PLAY", etc.).
     */
    std::string screenTypeToString(ScreenType screen) const;
};
