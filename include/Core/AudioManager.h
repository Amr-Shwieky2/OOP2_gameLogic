#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <memory>
#include <string>

/**
 * @class AudioManager
 * @brief Singleton class responsible for handling all audio operations (music and sound effects).
 *
 * Provides methods to load, play, stop, pause, and adjust volume for both music and sound effects.
 * Supports global volume control through master/music/SFX levels.
 */
class AudioManager {
public:
    /**
     * @brief Get the singleton instance of AudioManager.
     */
    static AudioManager& instance();

    // Volume control
    void setMasterVolume(float volume);   ///< Set master volume (0–100)
    void setMusicVolume(float volume);    ///< Set music volume (0–100)
    void setSFXVolume(float volume);      ///< Set SFX volume (0–100)

    float getMasterVolume() const;        ///< Get current master volume
    float getMusicVolume() const;         ///< Get current music volume
    float getSFXVolume() const;           ///< Get current SFX volume

    /**
     * @brief Reset all volume levels to 100%.
     */
    void resetAudioSystem();

    // Music playback
    bool loadMusic(const std::string& name, const std::string& filePath); ///< Load music from file
    void playMusic(const std::string& name, bool loop = true);            ///< Play music by name
    void pauseMusic();                                                    ///< Pause current music
    void resumeMusic();                                                   ///< Resume paused music
    void stopMusic();                                                     ///< Stop current music

    // Sound effects
    bool loadSound(const std::string& name, const std::string& filePath); ///< Load sound effect
    void playSound(const std::string& name);                              ///< Play one-shot sound effect
    void playSoundLoop(const std::string& name);                          ///< Play looping sound effect
    void stopSound(const std::string& name);                              ///< Stop specific sound effect
    void stopAllSounds();                                                 ///< Stop all sounds and music
    void stopAllSoundsExcept(const std::string& soundName);               ///< Stop all except given sound

private:
    AudioManager(); // Private constructor
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    /**
     * @brief Compute the final volume taking master volume into account.
     * @param baseVolume The music or SFX volume.
     * @return Adjusted volume after applying master level.
     */
    float getEffectiveVolume(float baseVolume) const;

    void updateMusicVolume(); ///< Internal method to update volume of currently playing music
    void updateSFXVolume();   ///< Internal method to update volume of all sound effects

private:
    float m_masterVolume;
    float m_musicVolume;
    float m_sfxVolume;

    std::unordered_map<std::string, std::unique_ptr<sf::Music>> m_music;
    std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::unordered_map<std::string, sf::Sound> m_sounds;

    sf::Music* m_currentMusic = nullptr;
};
