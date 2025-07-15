#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include "../Settings/SettingsInputHandler.h"
#include "../Settings/SettingsCommandExecutor.h"
#include "../Settings/SettingsAutoSaveManager.h"
#include "../Settings/SettingsEventLogger.h"
#include "../UI/VolumeControlPanel.h"

/**
 * @brief Coordinates input, command execution, logging, and auto-saving in the settings screen.
 *
 * This class follows the Single Responsibility Principle (SRP) by safely integrating multiple
 * subsystems related to the settings screen without using callbacks. All operations are executed
 * explicitly and safely (e.g., during destruction or Escape key handling).
 *
 * Key responsibilities:
 * - Handles keyboard input using polling (e.g., Escape, Ctrl+U, Ctrl+R, Ctrl+H)
 * - Executes undo/redo/history commands through SettingsCommandExecutor
 * - Manages auto-saving logic (enable, delay, check unsaved changes)
 * - Manages logging for user actions
 * - Allows graceful shutdown with no risk of dangling callbacks
 */
class SettingsCommandHandler {
public:
    /**
     * @brief Constructs the handler and initializes all internal subsystems.
     *
     * Initialization is safe and logged. No external state is modified.
     */
    SettingsCommandHandler();

    /**
     * @brief Safely destroys all components in the correct order.
     *
     * Disables input and ensures that no callbacks remain active.
     */
    ~SettingsCommandHandler();

    /**
     * @brief Main event entry point for keyboard handling.
     *
     * @param event The SFML keyboard event to process.
     * @return true if Escape key was pressed and screen should exit.
     */
    bool handleKeyboardInput(const sf::Event& event);

    /**
     * @brief Component configuration methods.
     *
     * These allow external systems (such as UI or screen manager) to register components.
     */
    void setVolumePanel(std::shared_ptr<VolumeControlPanel> panel);
    void enableAutoSave(bool enable);
    void setAutoSaveDelay(float seconds);
    void enableLogging(bool enable);

    /**
     * @brief State query methods.
     *
     * Used to check status of auto-save or whether unsaved changes exist.
     */
    bool isAutoSaveEnabled() const;
    bool hasUnsavedChanges() const;

private:
    // === Internal subsystems (each with single responsibility) ===
    std::unique_ptr<SettingsInputHandler> m_inputHandler;        ///< Handles raw input polling.
    std::unique_ptr<SettingsCommandExecutor> m_commandExecutor;  ///< Executes commands (undo/redo/history).
    std::unique_ptr<SettingsAutoSaveManager> m_autoSaveManager;  ///< Manages auto-save logic and state.
    std::unique_ptr<SettingsEventLogger> m_eventLogger;          ///< Logs user interactions and settings changes.

    bool m_isDestroying = false; ///< Flag to safely disable interaction during destruction.

    // === Internal helper methods ===

    /**
     * @brief Checks if all components are initialized and not in destruction state.
     */
    bool isComponentsValid() const;


    /**
     * @brief Handles Escape key input (triggers auto-save and screen exit).
     *
     * @return true if screen should exit.
     */
    bool handleEscapeInput();

    /**
     * @brief Handles Ctrl+U input (undo).
     *
     * @return true if undo succeeded.
     */
    bool handleUndoInput();

    /**
     * @brief Handles Ctrl+R input (redo).
     *
     * @return true if redo succeeded.
     */
    bool handleRedoInput();

    /**
     * @brief Handles Ctrl+H input (show command history).
     *
     * @return true if command history displayed successfully.
     */
    bool handleHistoryInput();
};
