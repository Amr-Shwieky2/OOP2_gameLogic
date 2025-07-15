#pragma once
#include <string>

/**
 * @brief Handles cleanup operations for the application during shutdown.
 *
 * This class follows the Single Responsibility Principle (SRP) and is responsible
 * for gracefully shutting down subsystems such as audio, settings, and temporary resources.
 *
 * Cleanup order is important and follows a safe shutdown sequence.
 */
class AppCleanupManager {
public:
    /**
     * @brief Constructs a new AppCleanupManager instance.
     */
    AppCleanupManager() = default;

    /**
     * @brief Destroys the AppCleanupManager instance.
     */
    ~AppCleanupManager() = default;

    /**
     * @brief Performs all application cleanup steps in a safe order.
     *
     * This includes stopping audio, saving settings, releasing resources, and cleaning temporary files.
     */
    void performCleanup();

private:
    // === Cleanup sub-steps ===

    /**
     * @brief Stops all audio playback and shuts down audio systems.
     */
    void cleanupAudioSystem();

    /**
     * @brief Saves user-specific settings (e.g., volume levels) to persistent storage.
     */
    void saveUserSettings();

    /**
     * @brief Cleans up resource managers (textures, fonts, sounds).
     *
     * Even though they follow RAII, this method logs resource cleanup info for debugging.
     */
    void cleanupResources();

    /**
     * @brief Cleans up any temporary or cached files created during runtime.
     *
     * This can include logs, temp saves, or runtime cache (if implemented).
     */
    void cleanupTempFiles();

    // === Error handling ===

    /**
     * @brief Logs and handles errors that occur during a specific cleanup operation.
     *
     * @param operation The name of the cleanup operation.
     * @param e The exception that was thrown.
     */
    void handleCleanupError(const std::string& operation, const std::exception& e);

    /**
     * @brief Logs the result (success/failure) of a cleanup operation.
     *
     * @param operation Name of the operation.
     * @param success True if the operation succeeded, false otherwise.
     */
    void logCleanupOperation(const std::string& operation, bool success);
};
