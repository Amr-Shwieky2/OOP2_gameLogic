#include "../../include/Commands/ExitGameCommand.h"
#include <cstdlib> 

//-------------------------------------------------------------------------------------
ExitGameCommand::ExitGameCommand() {}
//-------------------------------------------------------------------------------------
void ExitGameCommand::execute() {
    m_hasExecuted = true;
    std::cout << "Goodbye!" << std::endl;
    std::exit(0);  
}
//-------------------------------------------------------------------------------------
void ExitGameCommand::undo() {
    std::cout << "ExitGameCommand: Cannot undo exit command!" << std::endl;
    if (m_hasExecuted) {
        std::cout << "ExitGameCommand: Application would have already terminated" << std::endl;
    }
}
//-------------------------------------------------------------------------------------
bool ExitGameCommand::canUndo() const {
    return false;
}
//-------------------------------------------------------------------------------------
std::string ExitGameCommand::getName() const {
    return "ExitGame";
}
//-------------------------------------------------------------------------------------