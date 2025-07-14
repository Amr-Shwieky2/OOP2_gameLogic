#pragma once
#include <memory>
#include <string>

// Forward declarations
class VolumeControlPanel;

/**
 * @brief Responsible for auto-save functionality in Settings only
 * Single Responsibility: Auto-save functionality and settings persistence
 */
class SettingsAutoSaveManager {
public:
    SettingsAutoSaveManager();
    ~SettingsAutoSaveManager() = default;

    /**
     * @brief Configure auto-save settings
     */
    void enableAutoSave(bool enable) { m_autoSaveEnabled = enable; }
    void setAutoSaveDelay(float seconds) { m_autoSaveDelay = seconds; }

    bool isAutoSaveEnabled() const { return m_autoSaveEnabled; }
    float getAutoSaveDelay() const { return m_autoSaveDelay; }

    /**
     * @brief Register panels for auto-save
     */
    void setVolumePanel(std::shared_ptr<VolumeControlPanel> panel);

    /**
     * @brief Save operations
     */
    void saveSettingsBeforeExit();
    void performAutoSave();
    void forceSaveAllSettings();

    /**
     * @brief Check for changes
     */
    bool hasUnsavedChanges() const;
    bool shouldSaveVolumeSettings() const;

private:
    // Auto-save configuration
    bool m_autoSaveEnabled = true;
    float m_autoSaveDelay = 0.5f; // seconds

    // Panel references using weak_ptr for safe memory management
    std::weak_ptr<VolumeControlPanel> m_volumePanel;

    /**
     * @brief Different save operations
     */
    bool saveVolumeSettings();
    void handleSaveError(const std::string& settingType, const std::exception& e);

    /**
     * @brief Log save operations
     */
    void logSaveOperation(const std::string& operation, bool success);
};