#include "../../include/Core/CommandInvoker.h"

CommandInvoker::CommandInvoker() : m_currentIndex(-1) {
    std::cout << "CommandInvoker: Initialized" << std::endl;
}

void CommandInvoker::execute(std::unique_ptr<ICommand> command) {
    if (!command) {
        return;
    }
    // Execute the command
    command->execute();

    // Only save undoable commands to history
    if (command->canUndo()) {
        // Remove any commands after current position (clears redo stack)
        if (m_currentIndex + 1 < m_commandHistory.size()) {
            m_commandHistory.erase(m_commandHistory.begin() + m_currentIndex + 1,
                m_commandHistory.end());
        }

        // Add new command to history
        m_commandHistory.push_back(std::move(command));
        m_currentIndex = m_commandHistory.size() - 1;

        // Clean up if history is too large
        cleanupHistory();
    }
}

bool CommandInvoker::undo() {
    if (!canUndo()) {
        return false;
    }

    // Get current command and undo it
    auto& command = m_commandHistory[m_currentIndex];

    command->undo();
    m_currentIndex--;  // Move backward in history
    return true;
}

bool CommandInvoker::redo() {
    if (!canRedo()) {
        return false;
    }

    // Move forward in history and re-execute command
    m_currentIndex++;
    auto& command = m_commandHistory[m_currentIndex];
    command->execute();
    return true;
}

bool CommandInvoker::canUndo() const {
    // Can undo if we have commands and current index is valid
    return m_currentIndex >= 0 && !m_commandHistory.empty();
}

bool CommandInvoker::canRedo() const {
    // Can redo if there are commands ahead of current position
    return m_currentIndex + 1 < m_commandHistory.size();
}

void CommandInvoker::clearHistory() {
    m_commandHistory.clear();
    m_currentIndex = -1;
}

void CommandInvoker::printHistory() const {
    for (size_t i = 0; i < m_commandHistory.size(); i++) {
        std::string marker = (i == m_currentIndex) ? " <- Current" : "";
        std::cout << "   [" << i << "] " << m_commandHistory[i]->getName() << marker << std::endl;
    }
}

void CommandInvoker::cleanupHistory() {
    if (m_commandHistory.size() > MAX_HISTORY_SIZE) {
        // Keep only the last MAX_HISTORY_SIZE commands
        size_t removeCount = m_commandHistory.size() - MAX_HISTORY_SIZE;
        m_commandHistory.erase(m_commandHistory.begin(),
            m_commandHistory.begin() + removeCount);

        // Adjust current index
        m_currentIndex -= removeCount;
    }
}