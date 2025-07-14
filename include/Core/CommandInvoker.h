#pragma once
#include "ICommand.h"
#include <memory>
#include <vector>
#include <iostream>

/**
 * @brief Command Pattern Invoker - executes commands and manages Undo/Redo
 *
 * Acts as the central controller for all commands in the application.
 * Maintains command history and provides undo/redo functionality.
 */
class CommandInvoker {
public:
    CommandInvoker();
    ~CommandInvoker() = default;

    // Execute new command and add to history if undoable
    void execute(std::unique_ptr<ICommand> command);

    // Undo last executed command
    bool undo();

    // Redo previously undone command  
    bool redo();

    // Check if undo operation is possible
    bool canUndo() const;

    // Check if redo operation is possible
    bool canRedo() const;

    // Clear all command history
    void clearHistory();

    // Debug: print current command history
    void printHistory() const;

private:
    // History of executed commands
    std::vector<std::unique_ptr<ICommand>> m_commandHistory;

    // Current position in command history (-1 = no commands)
    int m_currentIndex;

    // Memory limit: maximum commands to keep in history
    static const size_t MAX_HISTORY_SIZE = 50;

    // Remove old commands when history gets too large
    void cleanupHistory();
};