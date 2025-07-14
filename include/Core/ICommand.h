#pragma once
#include <string>

/**
 * @brief Command Pattern Interface - converts actions into objects
 *
 * Instead of calling functions directly, wrap them in command objects.
 * This enables undo/redo, command queuing, and logging.
 */
class ICommand {
public:
    virtual ~ICommand() = default;

    // The main action - what this command actually does
    virtual void execute() = 0;

    // Reverse the command's effects (optional)
    virtual void undo() {}

    // Can this command be undone?
    virtual bool canUndo() const { return false; }

    // Name for debugging and history display
    virtual std::string getName() const { return "Unknown Command"; }
};