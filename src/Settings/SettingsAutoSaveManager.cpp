#include "../UI/VolumeControlPanel.h"
#include <iostream>
#include "../Settings/SettingsAutoSaveManager.h"

//-------------------------------------------------------------------------------------
SettingsAutoSaveManager::SettingsAutoSaveManager() {}
//-------------------------------------------------------------------------------------
void SettingsAutoSaveManager::setVolumePanel(std::shared_ptr<VolumeControlPanel> panel) {
    if (panel) {
        m_volumePanel = panel;
    }
}
//-------------------------------------------------------------------------------------
void SettingsAutoSaveManager::saveSettingsBeforeExit() {
    if (!m_autoSaveEnabled) {
        return;
    }

    bool volumeSaved = false;

    try {
        // Save volume settings
        volumeSaved = saveVolumeSettings();

        if (volumeSaved) {
            logSaveOperation("Volume settings", true);
        }

    }
    catch (const std::exception& e) {
        handleSaveError("Settings", e);
    }
}
//-------------------------------------------------------------------------------------
void SettingsAutoSaveManager::performAutoSave() {
    if (!m_autoSaveEnabled) {
        return;
    }

    try {
        saveSettingsBeforeExit();
        logSaveOperation("Auto-save", true);

    }
    catch (const std::exception& e) {
        handleSaveError("Auto-save", e);
        logSaveOperation("Auto-save", false);
    }
}
//-------------------------------------------------------------------------------------
void SettingsAutoSaveManager::forceSaveAllSettings() {
    try {
        // Force save regardless of auto-save setting
        bool originalAutoSave = m_autoSaveEnabled;
        m_autoSaveEnabled = true;

        saveSettingsBeforeExit();

        m_autoSaveEnabled = originalAutoSave;
    }
    catch (const std::exception& e) {
        handleSaveError("Force save", e);
    }
}
//-------------------------------------------------------------------------------------
bool SettingsAutoSaveManager::hasUnsavedChanges() const {
    try {
        return shouldSaveVolumeSettings();

    }
    catch (const std::exception& e) {
        std::cout << "SettingsAutoSaveManager: Error checking unsaved changes: " << e.what() << std::endl;
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool SettingsAutoSaveManager::shouldSaveVolumeSettings() const {
    if (auto panel = m_volumePanel.lock()) {
        try {
            return panel->hasChanged();
        }
        catch (const std::exception& e) {
            std::cout << "SettingsAutoSaveManager: Error checking volume changes: " << e.what() << std::endl;
            return false;
        }
    }
    return false;
}
//-------------------------------------------------------------------------------------
bool SettingsAutoSaveManager::saveVolumeSettings() {
    if (auto panel = m_volumePanel.lock()) {
        try {
            if (panel->hasChanged()) {
                panel->saveSettings();
                return true;
            }
            else {
                return false;
            }
        }
        catch (const std::exception& e) {
            handleSaveError("Volume settings", e);
            return false;
        }
    }
    else {
        std::cout << "SettingsAutoSaveManager: Volume panel not available for saving" << std::endl;
        return false;
    }
}
//-------------------------------------------------------------------------------------
void SettingsAutoSaveManager::handleSaveError(const std::string& settingType, const std::exception& e) {
    std::cout << "SettingsAutoSaveManager: Error saving " << settingType
        << ": " << e.what() << std::endl;
}
//-------------------------------------------------------------------------------------
void SettingsAutoSaveManager::logSaveOperation(const std::string& operation, bool success) {
    std::string status = success ? "SUCCESS" : "FAILED";
    std::cout << "SettingsAutoSaveManager: " << operation << " - " << status << std::endl;
}
//-------------------------------------------------------------------------------------