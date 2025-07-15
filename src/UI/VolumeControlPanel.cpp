#include "VolumeControlPanel.h"
#include <iostream>

//-------------------------------------------------------------------------------------
VolumeControlPanel::VolumeSlider::VolumeSlider(const std::string& labelText,
    const std::string& sliderType,
    const sf::Font& font,
    sf::Vector2f position)
    : type(sliderType) {
    label.setFont(font);
    label.setString(labelText);
    label.setCharacterSize(28);
    label.setFillColor(sf::Color(139, 69, 19));
    label.setPosition(position);

    sf::Vector2f sliderPos(position.x + 200, position.y);
    slider = std::make_unique<Slider>(sliderPos, sf::Vector2f(250, 35), 0.0f, 100.0f);

    value.setFont(font);
    value.setCharacterSize(24);
    value.setFillColor(sf::Color(139, 69, 19));
    value.setPosition(position.x + 470, position.y + 5);

    updateValueText();
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::VolumeSlider::updateValueText() {
    if (slider) {
        value.setString(std::to_string(static_cast<int>(slider->getValue())) + "%");
    }
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::VolumeSlider::setVolume(float volume) {
    if (slider) {
        slider->setValue(volume);
        updateValueText();
    }
}
//-------------------------------------------------------------------------------------
float VolumeControlPanel::VolumeSlider::getVolume() const {
    return slider ? slider->getValue() : 0.0f;
}
//-------------------------------------------------------------------------------------
VolumeControlPanel::VolumeControlPanel(const sf::Font& font, AudioManager& audio, AudioSettings& settings)
    : m_position(350, 250), m_audioManager(audio), m_audioSettings(settings) {

    setupMasterVolumeSlider(font);
    setupMusicVolumeSlider(font);
    setupSFXVolumeSlider(font);

    refreshFromAudioManager();
}
//-------------------------------------------------------------------------------------
VolumeControlPanel::~VolumeControlPanel() {}

void VolumeControlPanel::setupMasterVolumeSlider(const sf::Font& font) {
    sf::Vector2f position = calculateSliderPosition(0);
    m_masterVolume = std::make_unique<VolumeSlider>("Volume (Master):", "master", font, position);

    if (m_masterVolume->slider) {
        m_masterVolume->slider->setOnValueChanged([this](float value) {
            onVolumeChanged("master", value);
            });
    }
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::setupMusicVolumeSlider(const sf::Font& font) {
    sf::Vector2f position = calculateSliderPosition(1);
    m_musicVolume = std::make_unique<VolumeSlider>("Volume (Music):", "music", font, position);

    if (m_musicVolume->slider) {
        m_musicVolume->slider->setOnValueChanged([this](float value) {
            onVolumeChanged("music", value);
            });
    }
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::setupSFXVolumeSlider(const sf::Font& font) {
    sf::Vector2f position = calculateSliderPosition(2);
    m_sfxVolume = std::make_unique<VolumeSlider>("Volume (SFX):", "sfx", font, position);

    if (m_sfxVolume->slider) {
        m_sfxVolume->slider->setOnValueChanged([this](float value) {
            onVolumeChanged("sfx", value);
            });
    }
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::update(float deltaTime) {
    if (m_masterVolume && m_masterVolume->slider) m_masterVolume->slider->update(deltaTime);
    if (m_musicVolume && m_musicVolume->slider) m_musicVolume->slider->update(deltaTime);
    if (m_sfxVolume && m_sfxVolume->slider) m_sfxVolume->slider->update(deltaTime);
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::render(sf::RenderWindow& window) {
    if (m_masterVolume) {
        window.draw(m_masterVolume->label);
        if (m_masterVolume->slider) m_masterVolume->slider->render(window);
        window.draw(m_masterVolume->value);
    }
    if (m_musicVolume) {
        window.draw(m_musicVolume->label);
        if (m_musicVolume->slider) m_musicVolume->slider->render(window);
        window.draw(m_musicVolume->value);
    }
    if (m_sfxVolume) {
        window.draw(m_sfxVolume->label);
        if (m_sfxVolume->slider) m_sfxVolume->slider->render(window);
        window.draw(m_sfxVolume->value);
    }
}
//-------------------------------------------------------------------------------------
bool VolumeControlPanel::handleMouseEvent(const sf::Event& event) {
    bool handled = false;

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        if (m_masterVolume && m_masterVolume->slider) handled |= m_masterVolume->slider->handleMouseMove(mousePos);
        if (m_musicVolume && m_musicVolume->slider) handled |= m_musicVolume->slider->handleMouseMove(mousePos);
        if (m_sfxVolume && m_sfxVolume->slider) handled |= m_sfxVolume->slider->handleMouseMove(mousePos);
    }
    else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (m_masterVolume && m_masterVolume->slider) handled |= m_masterVolume->slider->handleMousePressed(mousePos);
        if (m_musicVolume && m_musicVolume->slider) handled |= m_musicVolume->slider->handleMousePressed(mousePos);
        if (m_sfxVolume && m_sfxVolume->slider) handled |= m_sfxVolume->slider->handleMousePressed(mousePos);
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        if (m_masterVolume && m_masterVolume->slider) handled |= m_masterVolume->slider->handleMouseReleased();
        if (m_musicVolume && m_musicVolume->slider) handled |= m_musicVolume->slider->handleMouseReleased();
        if (m_sfxVolume && m_sfxVolume->slider) handled |= m_sfxVolume->slider->handleMouseReleased();
    }

    return handled;
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::refreshFromAudioManager() {
    m_masterVolume->setVolume(m_audioSettings.masterVolume);
    m_musicVolume->setVolume(m_audioSettings.musicVolume);
    m_sfxVolume->setVolume(m_audioSettings.sfxVolume);
    m_hasChanged = false;
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::saveSettings() {
    AudioSettingsManager::save(m_audioSettings);
    m_hasChanged = false;
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::onVolumeChanged(const std::string& type, float value) {
    applyVolumeToAudioManager(type, value);
    updateAllValueTexts();
    m_hasChanged = true;
}
//-------------------------------------------------------------------------------------
sf::Vector2f VolumeControlPanel::calculateSliderPosition(int index) const {
    return sf::Vector2f(m_position.x, m_position.y + (index * m_spacing));
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::updateAllValueTexts() {
    if (m_masterVolume) m_masterVolume->updateValueText();
    if (m_musicVolume) m_musicVolume->updateValueText();
    if (m_sfxVolume) m_sfxVolume->updateValueText();
}
//-------------------------------------------------------------------------------------
void VolumeControlPanel::applyVolumeToAudioManager(const std::string& type, float value) {
    if (type == "master") {
        m_audioSettings.masterVolume = value;
        m_audioManager.setMasterVolume(value);
    }
    else if (type == "music") {
        m_audioSettings.musicVolume = value;
        m_audioManager.setMusicVolume(value);
    }
    else if (type == "sfx") {
        m_audioSettings.sfxVolume = value;
        m_audioManager.setSFXVolume(value);
    }
}
//-------------------------------------------------------------------------------------