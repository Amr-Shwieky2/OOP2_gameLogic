#include "../Core/AudioManager.h"
#include <iostream>
#include <algorithm>

AudioManager& AudioManager::instance() {
    static AudioManager instance;
    return instance;
}

AudioManager::AudioManager()
    : m_masterVolume(100.0f), m_musicVolume(100.0f), m_sfxVolume(100.0f) {
}

void AudioManager::setMasterVolume(float volume) {
    m_masterVolume = std::clamp(volume, 0.0f, 100.0f);
    updateMusicVolume();
    updateSFXVolume();
}

void AudioManager::setMusicVolume(float volume) {
    m_musicVolume = std::clamp(volume, 0.0f, 100.0f);
    updateMusicVolume();
}

void AudioManager::setSFXVolume(float volume) {
    m_sfxVolume = std::clamp(volume, 0.0f, 100.0f);
    updateSFXVolume();
}

float AudioManager::getMasterVolume() const {
    return m_masterVolume;
}

float AudioManager::getMusicVolume() const {
    return m_musicVolume;
}

float AudioManager::getSFXVolume() const {
    return m_sfxVolume;
}

bool AudioManager::loadMusic(const std::string& name, const std::string& filePath) {
    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(filePath)) {
        std::cerr << "Failed to load music: " << filePath << std::endl;
        return false;
    }
    m_music[name] = std::move(music);
    return true;
}

void AudioManager::playMusic(const std::string& name, bool loop) {
    auto it = m_music.find(name);
    if (it != m_music.end()) {
        if (m_currentMusic) m_currentMusic->stop();
        m_currentMusic = it->second.get();
        m_currentMusic->setLoop(loop);
        m_currentMusic->setVolume(getEffectiveVolume(m_musicVolume));
        m_currentMusic->play();
    }
}

void AudioManager::stopMusic() {
    if (m_currentMusic) m_currentMusic->stop();
    m_currentMusic = nullptr;
}

void AudioManager::pauseMusic() {
    if (m_currentMusic) m_currentMusic->pause();
}

void AudioManager::resumeMusic() {
    if (m_currentMusic) m_currentMusic->play();
}

bool AudioManager::loadSound(const std::string& name, const std::string& filePath) {
    if (!m_soundBuffers[name].loadFromFile(filePath)) {
        std::cerr << "Failed to load sound: " << filePath << std::endl;
        return false;
    }
    m_sounds[name].setBuffer(m_soundBuffers[name]);
    return true;
}

void AudioManager::playSound(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        it->second.setVolume(getEffectiveVolume(m_sfxVolume));
        it->second.play();
    }
}

void AudioManager::updateMusicVolume() {
    if (m_currentMusic) {
        m_currentMusic->setVolume(getEffectiveVolume(m_musicVolume));
    }
    for (auto& [name, music] : m_music) {
        if (music->getStatus() == sf::Music::Playing) {
            music->setVolume(getEffectiveVolume(m_musicVolume));
        }
    }
}

void AudioManager::updateSFXVolume() {
    for (auto& [name, sound] : m_sounds) {
        sound.setVolume(getEffectiveVolume(m_sfxVolume));
    }
}

void AudioManager::resetAudioSystem() {
    setMasterVolume(100.0f);
    setMusicVolume(100.0f);
    setSFXVolume(100.0f);
}

float AudioManager::getEffectiveVolume(float baseVolume) const {
    return (baseVolume / 100.0f) * (m_masterVolume / 100.0f) * 100.0f;
}

void AudioManager::stopAllSounds() {
    for (auto& [key, sound] : m_sounds) {
        sound.stop();
    }

    for (auto& [key, music] : m_music) {
        if (music) {
            music->stop();
        }
    }

    m_currentMusic = nullptr;
}

void AudioManager::stopAllSoundsExcept(const std::string& soundName) {
    for (auto& [key, sound] : m_sounds) {
        if (key != soundName) {
            sound.stop();
        }
    }

    for (auto& [key, music] : m_music) {
        if (music && key != soundName) {
            music->stop();
        }
    }

    if (m_currentMusic && m_music.find(soundName) != m_music.end()) {
        m_currentMusic = m_music[soundName].get();
    }
    else {
        m_currentMusic = nullptr;
    }
}
