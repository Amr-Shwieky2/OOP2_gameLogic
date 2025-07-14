#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "../Core/UITheme.h"
#include "Slider.h"

class SettingsUIFactory {
public:
    struct VolumeControls {
        sf::Text label;
        std::unique_ptr<Slider> slider;
        sf::Text value;
    };

    // Existing functions (keep as they are)
    static sf::Text createTitle(const sf::String& text, sf::Vector2f position,
        const sf::Font& font, const UITheme::Colors& colors);

    static sf::Text createLabel(const sf::String& text, sf::Vector2f position,
        const sf::Font& font, const UITheme::Colors& colors,
        int characterSize = 28);

    static VolumeControls createVolumeControl(const sf::String& labelText,
        sf::Vector2f position,
        const sf::Font& font,
        const UITheme::Colors& colors,
        const UITheme::Layout& layout,
        float initialValue = 100.0f);

private:
    SettingsUIFactory() = default;
};