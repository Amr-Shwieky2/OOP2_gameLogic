#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include "Slider.h"
#include "AudioManager.h"
#include "AudioSettingsManager.h"

class VolumeControlPanel {
public:
    VolumeControlPanel(const sf::Font& font, AudioManager& audioManager, AudioSettings& settings);
    ~VolumeControlPanel();

    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    bool handleMouseEvent(const sf::Event& event);
    void refreshFromAudioManager();
    void saveSettings();
    bool hasChanged() const { return m_hasChanged; }

private:
    struct VolumeSlider {
        std::string type;
        sf::Text label;
        sf::Text value;
        std::unique_ptr<Slider> slider;

        VolumeSlider(const std::string& labelText,
            const std::string& sliderType,
            const sf::Font& font,
            sf::Vector2f position);

        void updateValueText();
        void setVolume(float volume);
        float getVolume() const;
    };

    sf::Vector2f m_position;
    float m_spacing = 100.0f;
    bool m_hasChanged = false;

    AudioManager& m_audioManager;
    AudioSettings& m_audioSettings;

    std::unique_ptr<VolumeSlider> m_masterVolume;
    std::unique_ptr<VolumeSlider> m_musicVolume;
    std::unique_ptr<VolumeSlider> m_sfxVolume;

    sf::Vector2f calculateSliderPosition(int index) const;
    void setupMasterVolumeSlider(const sf::Font& font);
    void setupMusicVolumeSlider(const sf::Font& font);
    void setupSFXVolumeSlider(const sf::Font& font);
    void updateAllValueTexts();
    void onVolumeChanged(const std::string& type, float value);
    void applyVolumeToAudioManager(const std::string& type, float value);
};
