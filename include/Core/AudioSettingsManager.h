#pragma once
#include <string>

/**
 * @struct AudioSettings
 * @brief Structure that holds user-configurable audio settings.
 *
 * This includes master, music, and SFX volumes as well as menu-specific audio preferences.
 */
struct AudioSettings {
    float masterVolume = 100.0f;      ///< Global volume affecting all sounds
    float musicVolume = 100.0f;       ///< Volume for background music
    float sfxVolume = 100.0f;         ///< Volume for sound effects
    bool menuSoundsEnabled = true;    ///< Toggle for enabling/disabling menu sounds
    float menuSoundVolume = 100.0f;   ///< Specific volume for menu-related sounds
};

/**
 * @class AudioSettingsManager
 * @brief Provides static methods to save and load audio settings to/from file.
 *
 * Handles serialization and deserialization of AudioSettings to a simple text file format.
 */
class AudioSettingsManager {
public:
    /**
     * @brief Save audio settings to file.
     * @param settings The AudioSettings object to save.
     * @param filename Name of the file to save into. Default is "settings.txt".
     * @return true if saved successfully, false otherwise.
     */
    static bool save(const AudioSettings& settings, const std::string& filename = "settings.txt");

    /**
     * @brief Load audio settings from file.
     * @param settings The AudioSettings object to populate.
     * @param filename Name of the file to load from. Default is "settings.txt".
     * @return true if loaded successfully, false otherwise.
     */
    static bool load(AudioSettings& settings, const std::string& filename = "settings.txt");
};
