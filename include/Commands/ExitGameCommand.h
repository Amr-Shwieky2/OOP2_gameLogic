#pragma once
#include "../Core/ICommand.h"
#include <string>
#include <iostream>

/**
 * @brief Command that exits the game immediately when executed.
 *
 * This class implements the Command Pattern to encapsulate the logic for exiting
 * the game. Once executed, the command calls std::exit(0) and terminates the application.
 *
 * It is a non-undoable operation and should be used with caution (e.g., via a "Quit" button).
 */
class ExitGameCommand : public ICommand {
public:
    /**
     * @brief Constructs the ExitGameCommand.
     */
    ExitGameCommand();

    // === ICommand interface implementation ===

    /**
     * @brief Executes the command and immediately terminates the application.
     */
    void execute() override;

    /**
     * @brief Attempts to undo the exit command (not supported).
     *
     * This method only logs a message; the command is irreversible.
     */
    void undo() override;

    /**
     * @brief Indicates whether the command supports undo.
     *
     * @return Always returns false.
     */
    bool canUndo() const override;

    /**
     * @brief Returns the name of the command ("ExitGame").
     */
    std::string getName() const override;

private:
    bool m_hasExecuted = false;  ///< Tracks whether the command was executed (for debug/logging only).
};
