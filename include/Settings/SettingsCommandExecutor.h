#pragma once
#include "../Core/ICommand.h"
#include "../Commands/EscapeKeyCommand.h"
#include "../Config/ScreenTypes.h"
#include <memory>

/**
 * @brief Responsible for command execution in Settings screen only
 * Single Responsibility: Command execution and management
 */
class SettingsCommandExecutor {
public:
    SettingsCommandExecutor();
    ~SettingsCommandExecutor() = default;

    /**
     * @brief Execute Settings screen commands
     */
    void executeEscapeCommand();
    bool executeUndo();
    bool executeRedo();
    void executeHistoryCommand();

    /**
     * @brief Command history management
     */
    void clearCommandHistory();
    bool canUndo() const;
    bool canRedo() const;

    /**
     * @brief Safe command execution with error handling
     */
    void executeCommandSafely(std::unique_ptr<ICommand> command);

private:
    /**
     * @brief Create specific commands
     */
    std::unique_ptr<EscapeKeyCommand> createEscapeCommand();

    /**
     * @brief Handle command execution errors
     */
    void handleCommandError(const std::string& commandName, const std::exception& e);
};