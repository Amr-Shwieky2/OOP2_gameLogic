#include "AudioSettingsManager.h"
#include <fstream>
#include <iostream>

//-------------------------------------------------------------------------------------
bool AudioSettingsManager::save(const AudioSettings& settings, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to save audio settings\n";
        return false;
    }
    file << "master_volume=" << settings.masterVolume << "\n";
    file << "music_volume=" << settings.musicVolume << "\n";
    file << "sfx_volume=" << settings.sfxVolume << "\n";
    file << "menu_sounds_enabled=" << (settings.menuSoundsEnabled ? 1 : 0) << "\n";
    file << "menu_sound_volume=" << settings.menuSoundVolume << "\n";
    return true;
}
//-------------------------------------------------------------------------------------
bool AudioSettingsManager::load(AudioSettings& settings, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false; 
    }

    std::string line;
    while (std::getline(file, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string valueStr = line.substr(pos + 1);

        try {
            if (key == "master_volume")
                settings.masterVolume = std::stof(valueStr);
            else if (key == "music_volume")
                settings.musicVolume = std::stof(valueStr);
            else if (key == "sfx_volume")
                settings.sfxVolume = std::stof(valueStr);
            else if (key == "menu_sounds_enabled")
                settings.menuSoundsEnabled = std::stoi(valueStr) == 1;
            else if (key == "menu_sound_volume")
                settings.menuSoundVolume = std::stof(valueStr);
        }
        catch (const std::exception& e){
            std::cerr << "Error parsing line: " << line << std::endl;
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------