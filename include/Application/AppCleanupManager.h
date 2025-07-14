#pragma once
#include <string>

/**
 * @brief Application cleanup only 
 */
class AppCleanupManager {
public:
    AppCleanupManager() = default;
    ~AppCleanupManager() = default;

    // Main cleanup entry point
    void performCleanup();

private:
    // Individual cleanup operations
    void cleanupAudioSystem();
    void saveUserSettings();
    void cleanupResources();
    void cleanupTempFiles();

    // Error handling
    void handleCleanupError(const std::string& operation, const std::exception& e);
    void logCleanupOperation(const std::string& operation, bool success);
};
