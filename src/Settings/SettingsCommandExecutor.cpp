#include "../Settings/SettingsCommandExecutor.h"
#include "../Application/AppContext.h"
#include <iostream>

SettingsCommandExecutor::SettingsCommandExecutor() {}

void SettingsCommandExecutor::executeEscapeCommand() {
    try {
        auto escapeCommand = createEscapeCommand();
        executeCommandSafely(std::move(escapeCommand));

    }
    catch (const std::exception& e) {
        handleCommandError("EscapeCommand", e);
    }
}

bool SettingsCommandExecutor::executeUndo() {
    try {
        std::cout << "SettingsCommandExecutor: Executing Undo command" << std::endl;

        bool result = AppContext::instance().commandInvoker().undo();

        if (result) {
            std::cout << "SettingsCommandExecutor: Undo executed successfully" << std::endl;
        }
        else {
            std::cout << "SettingsCommandExecutor: No command available to undo" << std::endl;
        }

        return result;

    }
    catch (const std::exception& e) {
        handleCommandError("UndoCommand", e);
        return false;
    }
}

bool SettingsCommandExecutor::executeRedo() {
    try {
        std::cout << "SettingsCommandExecutor: Executing Redo command" << std::endl;

        bool result = AppContext::instance().commandInvoker().redo();

        if (result) {
            std::cout << "SettingsCommandExecutor: Redo executed successfully" << std::endl;
        }
        else {
            std::cout << "SettingsCommandExecutor: No command available to redo" << std::endl;
        }

        return result;

    }
    catch (const std::exception& e) {
        handleCommandError("RedoCommand", e);
        return false;
    }
}

void SettingsCommandExecutor::executeHistoryCommand() {
    try {
        AppContext::instance().commandInvoker().printHistory();
    }
    catch (const std::exception& e) {
        handleCommandError("HistoryCommand", e);
    }
}

void SettingsCommandExecutor::clearCommandHistory() {
    try {
        AppContext::instance().commandInvoker().clearHistory();

    }
    catch (const std::exception& e) {
        handleCommandError("ClearHistoryCommand", e);
    }
}

bool SettingsCommandExecutor::canUndo() const {
    try {
        return AppContext::instance().commandInvoker().canUndo();
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandExecutor: Error checking undo status: " << e.what() << std::endl;
        return false;
    }
}

bool SettingsCommandExecutor::canRedo() const {
    try {
        return AppContext::instance().commandInvoker().canRedo();
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandExecutor: Error checking redo status: " << e.what() << std::endl;
        return false;
    }
}

void SettingsCommandExecutor::executeCommandSafely(std::unique_ptr<ICommand> command) {
    if (!command) {
        return;
    }

    try {
        AppContext::instance().commandInvoker().execute(std::move(command));

    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandExecutor: Exception during command execution: " << e.what() << std::endl;
    }
}

std::unique_ptr<EscapeKeyCommand> SettingsCommandExecutor::createEscapeCommand() {
    return std::make_unique<EscapeKeyCommand>(
        ScreenType::SETTINGS,
        ScreenType::MENU
    );
}

void SettingsCommandExecutor::handleCommandError(const std::string& commandName, const std::exception& e) {
    std::cout << "SettingsCommandExecutor: Error executing " << commandName
        << ": " << e.what() << std::endl;
}