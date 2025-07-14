#include "../../include/Commands/ExitGameCommand.h"
#include <cstdlib> // for std::exit

ExitGameCommand::ExitGameCommand() {}

void ExitGameCommand::execute() {
    // Mark as executed before termination
    m_hasExecuted = true;

    std::cout << "Goodbye!" << std::endl;
    std::exit(0);  
}

void ExitGameCommand::undo() {
    // Exit commands cannot be reversed once executed
    std::cout << "ExitGameCommand: Cannot undo exit command!" << std::endl;

    // Log execution status for debugging
    if (m_hasExecuted) {
        std::cout << "ExitGameCommand: Application would have already terminated" << std::endl;
    }
}

bool ExitGameCommand::canUndo() const {
    // Exit operations are inherently irreversible
    return false;
}

std::string ExitGameCommand::getName() const {
    return "ExitGame";
}