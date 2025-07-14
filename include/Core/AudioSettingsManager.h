#pragma once
#include <string>

struct AudioSettings {
    float masterVolume = 100.0f;
    float musicVolume = 100.0f;
    float sfxVolume = 100.0f;
    bool menuSoundsEnabled = true;
    float menuSoundVolume = 100.0f;
};

class AudioSettingsManager {
public:
    static bool save(const AudioSettings& settings, const std::string& filename = "settings.txt");
    static bool load(AudioSettings& settings, const std::string& filename = "settings.txt");
};