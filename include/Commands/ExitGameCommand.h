#pragma once
#include "../Core/ICommand.h"
#include <string>
#include <iostream>

/**
 * @brief Command for game exit - non-undoable operation
 * Implements Command pattern for immediate application termination
 */
class ExitGameCommand : public ICommand {
public:
    ExitGameCommand();

    // ICommand interface implementation
    void execute() override;        // Terminate application immediately
    void undo() override;          // Not supported - logs warning
    bool canUndo() const override; // Always returns false
    std::string getName() const override; // Returns command name

private:
    bool m_hasExecuted = false;    // Track if command was executed
};
