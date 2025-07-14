#include "../../include/Commands/SettingsCommandHandler.h"
#include <iostream>

SettingsCommandHandler::SettingsCommandHandler() : m_isDestroying(false) {
    std::cout << "SettingsCommandHandler: Initializing safe coordinator..." << std::endl;

    try {
        // Create specialized components - each with single responsibility
        m_inputHandler = std::make_unique<SettingsInputHandler>();
        m_commandExecutor = std::make_unique<SettingsCommandExecutor>();
        m_autoSaveManager = std::make_unique<SettingsAutoSaveManager>();
        m_eventLogger = std::make_unique<SettingsEventLogger>();

        std::cout << "SettingsCommandHandler: All components initialized safely (no callbacks)" << std::endl;

    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error initializing components: " << e.what() << std::endl;
    }
}

SettingsCommandHandler::~SettingsCommandHandler() {
    try {
        m_isDestroying = true;
        std::cout << "SettingsCommandHandler: Starting safe destruction..." << std::endl;

        // Disable input handler first
        if (m_inputHandler) {
            m_inputHandler->setEnabled(false);
        }

        // Reset components in safe order - no callbacks to clear!
        m_eventLogger.reset();
        m_autoSaveManager.reset();
        m_commandExecutor.reset();
        m_inputHandler.reset();

        std::cout << "SettingsCommandHandler: Destroyed safely without callbacks" << std::endl;

    }
    catch (...) {
        // Silent cleanup - don't throw in destructor
    }
}

bool SettingsCommandHandler::handleKeyboardInput(const sf::Event& event) {
    if (m_isDestroying) {
        std::cout << "SettingsCommandHandler: Already destroying, ignoring input" << std::endl;
        return false;
    }

    if (!isComponentsValid()) {
        std::cout << "SettingsCommandHandler: Components invalid, ignoring input" << std::endl;
        return false;
    }

    try {
        // Check for specific keys directly - NO CALLBACKS!
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Escape:
                std::cout << "SettingsCommandHandler: Escape detected, processing..." << std::endl;
                return handleEscapeInput();  // Returns true = exit screen

            case sf::Keyboard::U:
                if (event.key.control) {
                    std::cout << "SettingsCommandHandler: Ctrl+U detected" << std::endl;
                    handleUndoInput();
                }
                break;

            case sf::Keyboard::R:
                if (event.key.control) {
                    std::cout << "SettingsCommandHandler: Ctrl+R detected" << std::endl;
                    handleRedoInput();
                }
                break;

            case sf::Keyboard::H:
                if (event.key.control) {
                    std::cout << "SettingsCommandHandler: Ctrl+H detected" << std::endl;
                    handleHistoryInput();
                }
                break;

            default:
                break;
            }
        }

        return false; // Continue on screen

    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error in handleKeyboardInput: " << e.what() << std::endl;
        return false;
    }
}

void SettingsCommandHandler::setVolumePanel(std::shared_ptr<VolumeControlPanel> panel) {
    if (m_isDestroying || !m_autoSaveManager) {
        return;
    }

    try {
        m_autoSaveManager->setVolumePanel(panel);
        std::cout << "SettingsCommandHandler: Volume panel registered successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error setting volume panel: " << e.what() << std::endl;
    }
}

void SettingsCommandHandler::enableAutoSave(bool enable) {
    if (m_isDestroying || !m_autoSaveManager) {
        return;
    }

    try {
        m_autoSaveManager->enableAutoSave(enable);
        std::cout << "SettingsCommandHandler: Auto-save " << (enable ? "enabled" : "disabled") << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error enabling auto-save: " << e.what() << std::endl;
    }
}

void SettingsCommandHandler::setAutoSaveDelay(float seconds) {
    if (m_isDestroying || !m_autoSaveManager) {
        return;
    }

    try {
        m_autoSaveManager->setAutoSaveDelay(seconds);
        std::cout << "SettingsCommandHandler: Auto-save delay set to " << seconds << " seconds" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error setting auto-save delay: " << e.what() << std::endl;
    }
}

void SettingsCommandHandler::enableLogging(bool enable) {
    if (m_isDestroying || !m_eventLogger) {
        return;
    }

    try {
        m_eventLogger->enableFileLogging(enable);
        m_eventLogger->enableConsoleLogging(enable);
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error configuring logging: " << e.what() << std::endl;
    }
}

bool SettingsCommandHandler::isAutoSaveEnabled() const {
    if (m_isDestroying || !m_autoSaveManager) {
        return false;
    }

    try {
        return m_autoSaveManager->isAutoSaveEnabled();
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error checking auto-save status: " << e.what() << std::endl;
        return false;
    }
}

bool SettingsCommandHandler::hasUnsavedChanges() const {
    if (m_isDestroying || !m_autoSaveManager) {
        return false;
    }

    try {
        return m_autoSaveManager->hasUnsavedChanges();
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error checking unsaved changes: " << e.what() << std::endl;
        return false;
    }
}

bool SettingsCommandHandler::isComponentsValid() const {
    return !m_isDestroying &&
        m_inputHandler &&
        m_commandExecutor &&
        m_autoSaveManager &&
        m_eventLogger;
}

void SettingsCommandHandler::safeLog(const std::string& message) {
    // نستخدم فقط console logging لتجنب use-after-free تماماً
    if (!m_isDestroying) {
        std::cout << "SettingsCommandHandler: " << message << std::endl;
    }
}

bool SettingsCommandHandler::handleEscapeInput() {
    if (m_isDestroying) {
        std::cout << "SettingsCommandHandler: Already destroying, cannot handle escape" << std::endl;
        return false;
    }

    if (!isComponentsValid()) {
        std::cout << "SettingsCommandHandler: Components invalid, cannot handle escape" << std::endl;
        return false;
    }

    try {
        std::cout << "SettingsCommandHandler: Processing Escape key..." << std::endl;

        // Auto-save before exit if enabled
        if (m_autoSaveManager && m_autoSaveManager->isAutoSaveEnabled()) {
            std::cout << "SettingsCommandHandler: Performing auto-save before exit..." << std::endl;
            m_autoSaveManager->saveSettingsBeforeExit();
        }

        // Execute escape command
        if (m_commandExecutor) {
            std::cout << "SettingsCommandHandler: Executing escape command..." << std::endl;
            m_commandExecutor->executeEscapeCommand();
        }

        std::cout << "SettingsCommandHandler: Escape handled successfully - signaling exit" << std::endl;
        return true; // Signal screen should exit

    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error handling escape: " << e.what() << std::endl;
        return false;
    }
}

bool SettingsCommandHandler::handleUndoInput() {
    if (m_isDestroying || !isComponentsValid()) {
        return false;
    }

    try {
        std::cout << "SettingsCommandHandler: Processing Undo..." << std::endl;
        bool success = m_commandExecutor->executeUndo();
        std::cout << "SettingsCommandHandler: Undo " << (success ? "succeeded" : "failed") << std::endl;
        return success;
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error handling undo: " << e.what() << std::endl;
        return false;
    }
}

bool SettingsCommandHandler::handleRedoInput() {
    if (m_isDestroying || !isComponentsValid()) {
        return false;
    }

    try {
        std::cout << "SettingsCommandHandler: Processing Redo..." << std::endl;
        bool success = m_commandExecutor->executeRedo();
        std::cout << "SettingsCommandHandler: Redo " << (success ? "succeeded" : "failed") << std::endl;
        return success;
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error handling redo: " << e.what() << std::endl;
        return false;
    }
}

bool SettingsCommandHandler::handleHistoryInput() {
    if (m_isDestroying || !isComponentsValid()) {
        return false;
    }

    try {
        std::cout << "SettingsCommandHandler: Processing History..." << std::endl;
        m_commandExecutor->executeHistoryCommand();
        std::cout << "SettingsCommandHandler: Command history displayed successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "SettingsCommandHandler: Error handling history: " << e.what() << std::endl;
        return false;
    }
}