#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <memory>

class AudioManager {
public:
    static AudioManager& instance(); // Singleton

    // Volume control
    void setMasterVolume(float volume);
    void setMusicVolume(float volume);
    void setSFXVolume(float volume);

    float getMasterVolume() const;
    float getMusicVolume() const;
    float getSFXVolume() const;

    // Music control
    bool loadMusic(const std::string& name, const std::string& filePath);
    void playMusic(const std::string& name, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    // Sound effects
    bool loadSound(const std::string& name, const std::string& filePath);
    void playSound(const std::string& name);

    void updateMusicVolume();
    void updateSFXVolume();

    void stopAllSounds();
    void resetAudioSystem();

private:
    AudioManager(); // Singleton constructor
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    float getEffectiveVolume(float baseVolume) const;

    void stopAllSoundsExcept(const std::string& soundName);

private:
    float m_masterVolume;
    float m_musicVolume;
    float m_sfxVolume;

    std::unordered_map<std::string, std::unique_ptr<sf::Music>> m_music;
    sf::Music* m_currentMusic = nullptr;

    std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::unordered_map<std::string, sf::Sound> m_sounds;
};
