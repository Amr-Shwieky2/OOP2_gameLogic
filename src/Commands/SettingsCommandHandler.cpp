#include "../../include/Commands/SettingsCommandHandler.h"

//-------------------------------------------------------------------------------------
SettingsCommandHandler::SettingsCommandHandler() : m_isDestroying(false) {
    try {
        m_inputHandler = std::make_unique<SettingsInputHandler>();
        m_commandExecutor = std::make_unique<SettingsCommandExecutor>();
        m_autoSaveManager = std::make_unique<SettingsAutoSaveManager>();
        m_eventLogger = std::make_unique<SettingsEventLogger>();
    }
    catch (const std::exception&) {
    }
}
//-------------------------------------------------------------------------------------
SettingsCommandHandler::~SettingsCommandHandler() {
    m_isDestroying = true;
    if (m_inputHandler) {
        m_inputHandler->setEnabled(false);
    }
    m_eventLogger.reset();
    m_autoSaveManager.reset();
    m_commandExecutor.reset();
    m_inputHandler.reset();
}
//-------------------------------------------------------------------------------------
bool SettingsCommandHandler::handleKeyboardInput(const sf::Event& event) {
    if (m_isDestroying || !isComponentsValid()) {
        return false;
    }

    try {
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Escape:
                return handleEscapeInput();
            case sf::Keyboard::U:
                if (event.key.control)
                    handleUndoInput();
                break;
            case sf::Keyboard::R:
                if (event.key.control)
                    handleRedoInput();
                break;
            case sf::Keyboard::H:
                if (event.key.control)
                    handleHistoryInput();
                break;
            default:
                break;
            }
        }
        return false;
    }
    catch (const std::exception&) {
        return false;
    }
}
//-------------------------------------------------------------------------------------
void SettingsCommandHandler::setVolumePanel(std::shared_ptr<VolumeControlPanel> panel) {
    if (m_isDestroying || !m_autoSaveManager) return;

    try {
        m_autoSaveManager->setVolumePanel(panel);
    }
    catch (const std::exception&) {}
}
//-------------------------------------------------------------------------------------
void SettingsCommandHandler::enableAutoSave(bool enable) {
    if (m_isDestroying || !m_autoSaveManager) return;

    try {
        m_autoSaveManager->enableAutoSave(enable);
    }
    catch (const std::exception&) {}
}
//-------------------------------------------------------------------------------------
void SettingsCommandHandler::setAutoSaveDelay(float seconds) {
    if (m_isDestroying || !m_autoSaveManager) return;

    try {
        m_autoSaveManager->setAutoSaveDelay(seconds);
    }
    catch (const std::exception&) {}
}
//-------------------------------------------------------------------------------------
void SettingsCommandHandler::enableLogging(bool enable) {
    if (m_isDestroying || !m_eventLogger) return;

    try {
        m_eventLogger->enableFileLogging(enable);
        m_eventLogger->enableConsoleLogging(enable);
    }
    catch (const std::exception&) {}
}
//-------------------------------------------------------------------------------------
bool SettingsCommandHandler::isAutoSaveEnabled() const {
    if (m_isDestroying || !m_autoSaveManager) return false;

    try {
        return m_autoSaveManager->isAutoSaveEnabled();
    }
    catch (const std::exception&) {
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool SettingsCommandHandler::hasUnsavedChanges() const {
    if (m_isDestroying || !m_autoSaveManager) return false;

    try {
        return m_autoSaveManager->hasUnsavedChanges();
    }
    catch (const std::exception&) {
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool SettingsCommandHandler::isComponentsValid() const {
    return !m_isDestroying &&
        m_inputHandler &&
        m_commandExecutor &&
        m_autoSaveManager &&
        m_eventLogger;
}
//-------------------------------------------------------------------------------------
bool SettingsCommandHandler::handleEscapeInput() {
    if (m_isDestroying || !isComponentsValid()) return false;

    try {
        if (m_autoSaveManager && m_autoSaveManager->isAutoSaveEnabled()) {
            m_autoSaveManager->saveSettingsBeforeExit();
        }

        if (m_commandExecutor) {
            m_commandExecutor->executeEscapeCommand();
        }

        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool SettingsCommandHandler::handleUndoInput() {
    if (m_isDestroying || !isComponentsValid()) return false;

    try {
        return m_commandExecutor->executeUndo();
    }
    catch (const std::exception&) {
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool SettingsCommandHandler::handleRedoInput() {
    if (m_isDestroying || !isComponentsValid()) return false;

    try {
        return m_commandExecutor->executeRedo();
    }
    catch (const std::exception&) {
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool SettingsCommandHandler::handleHistoryInput() {
    if (m_isDestroying || !isComponentsValid()) return false;

    try {
        m_commandExecutor->executeHistoryCommand();
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}
//-------------------------------------------------------------------------------------