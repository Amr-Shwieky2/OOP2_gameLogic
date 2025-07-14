#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "../Settings/SettingsInputHandler.h"
#include "../Settings/SettingsCommandExecutor.h"
#include "../Settings/SettingsAutoSaveManager.h"
#include "../Settings/SettingsEventLogger.h"
#include "../UI/VolumeControlPanel.h"

/**
 * @brief Safe Settings Command Handler - Uses polling instead of callbacks
 * Single Responsibility: Coordinates components safely without callbacks
 */
class SettingsCommandHandler {
public:
    SettingsCommandHandler();
    ~SettingsCommandHandler();

    /**
     * @brief Main entry point - returns true if should exit to menu
     * @param event SFML event
     * @return true if screen should exit (ESC was pressed)
     */
    bool handleKeyboardInput(const sf::Event& event);

    /**
     * @brief Configure components
     */
    void setVolumePanel(std::shared_ptr<VolumeControlPanel> panel);
    void enableAutoSave(bool enable);
    void setAutoSaveDelay(float seconds);
    void enableLogging(bool enable);

    /**
     * @brief State queries
     */
    bool isAutoSaveEnabled() const;
    bool hasUnsavedChanges() const;

private:
    // Specialized components - each has one responsibility
    std::unique_ptr<SettingsInputHandler> m_inputHandler;
    std::unique_ptr<SettingsCommandExecutor> m_commandExecutor;
    std::unique_ptr<SettingsAutoSaveManager> m_autoSaveManager;
    std::unique_ptr<SettingsEventLogger> m_eventLogger;

    bool m_isDestroying = false;  // Safety flag

    /**
     * @brief Safe helper methods
     */
    bool isComponentsValid() const;
    void safeLog(const std::string& message);
    bool handleEscapeInput();
    bool handleUndoInput();
    bool handleRedoInput();
    bool handleHistoryInput();
};