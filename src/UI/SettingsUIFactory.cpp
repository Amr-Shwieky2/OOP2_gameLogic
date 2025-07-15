#include "SettingsUIFactory.h"
#include "Slider.h"

//-------------------------------------------------------------------------------------
sf::Text SettingsUIFactory::createTitle(const sf::String& text, sf::Vector2f position,
    const sf::Font& font, const UITheme::Colors& colors) {

    sf::Text title;
    title.setFont(font);
    title.setString(text);
    title.setCharacterSize(48);
    title.setFillColor(colors.titleText);
    title.setPosition(position);
    title.setStyle(sf::Text::Bold);

    return title;
}
//-------------------------------------------------------------------------------------
sf::Text SettingsUIFactory::createLabel(const sf::String& text, sf::Vector2f position,
    const sf::Font& font, const UITheme::Colors& colors, int characterSize) {

    sf::Text label;
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(characterSize);
    label.setFillColor(colors.labelText);
    label.setPosition(position);

    return label;
}
//-------------------------------------------------------------------------------------
SettingsUIFactory::VolumeControls SettingsUIFactory::createVolumeControl(
    const sf::String& labelText,
    sf::Vector2f position,
    const sf::Font& font,
    const UITheme::Colors& colors,
    const UITheme::Layout& layout,
    float initialValue) {

    VolumeControls control;

    // Create label
    control.label = createLabel(labelText, position, font, colors);

    // Create slider
    sf::Vector2f sliderPos(position.x + 200, position.y);
    control.slider = std::make_unique<Slider>(sliderPos, layout.sliderSize, 0.0f, 100.0f);
    control.slider->setValue(initialValue);
    control.slider->setColors(colors.sliderBackground, colors.sliderFill, colors.sliderHandle);

    // Create value text
    control.value.setFont(font);
    control.value.setCharacterSize(24);
    control.value.setFillColor(colors.valueText);
    control.value.setPosition(position.x + 470, position.y + 5);
    control.value.setString(std::to_string(static_cast<int>(initialValue)) + "%");

    return control;
}
//-------------------------------------------------------------------------------------