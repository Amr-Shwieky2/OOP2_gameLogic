#include "../../include/UI/ButtonFactory.h"
#include "../../include/Application/AppContext.h"
#include <iostream>

//-------------------------------------------------------------------------------------
std::unique_ptr<ObservableButton> ButtonFactory::createAboutButton(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    std::shared_ptr<MenuButtonObserver> observer,
    const sf::Font& font) {

    auto button = std::make_unique<ObservableButton>(position, size, "ABOUT US", "about");
    setupButton(*button, observer, font, "AboutButton.png", sf::Color(150, 100, 200, 255));

    return button;
}
//-------------------------------------------------------------------------------------
std::unique_ptr<ObservableButton> ButtonFactory::createStartButton(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    std::shared_ptr<MenuButtonObserver> observer,
    const sf::Font& font) {

    auto button = std::make_unique<ObservableButton>(position, size, "START GAME", "start");
    setupButton(*button, observer, font, "StartButtonEnglish.png", sf::Color(80, 200, 80, 255));

    return button;
}
//-------------------------------------------------------------------------------------
std::unique_ptr<ObservableButton> ButtonFactory::createSettingsButton(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    std::shared_ptr<MenuButtonObserver> observer,
    const sf::Font& font) {
    auto button = std::make_unique<ObservableButton>(position, size, "SETTINGS", "settings");
    setupButton(*button, observer, font, "SettingsButtonEnglish.png", sf::Color(80, 80, 200, 255));

    return button;
}
//-------------------------------------------------------------------------------------
std::unique_ptr<ObservableButton> ButtonFactory::createHelpButton(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    std::shared_ptr<MenuButtonObserver> observer,
    const sf::Font& font) {

    auto button = std::make_unique<ObservableButton>(position, size, "HELP", "help");
    setupButton(*button, observer, font, "HelpButtonEnglish.png", sf::Color(200, 200, 80, 255));

    return button;
}
//-------------------------------------------------------------------------------------
std::unique_ptr<ObservableButton> ButtonFactory::createExitButton(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    std::shared_ptr<MenuButtonObserver> observer,
    const sf::Font& font) {

    auto button = std::make_unique<ObservableButton>(position, size, "EXIT", "exit");
    setupButton(*button, observer, font, "ExitButtonEnglish.png", sf::Color(200, 80, 80, 255));

    return button;
}
//-------------------------------------------------------------------------------------
std::unique_ptr<ObservableButton> ButtonFactory::createButton(
    ButtonType type,
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    std::shared_ptr<MenuButtonObserver> observer,
    const sf::Font& font) {

    switch (type) {
    case ButtonType::ABOUT:
        return createAboutButton(position, size, observer, font);
    case ButtonType::START:
        return createStartButton(position, size, observer, font);
    case ButtonType::SETTINGS:
        return createSettingsButton(position, size, observer, font);
    case ButtonType::HELP:
        return createHelpButton(position, size, observer, font);
    case ButtonType::EXIT:
        return createExitButton(position, size, observer, font);
    default:
        std::cout << "Factory: Unknown button type!" << std::endl;
        return nullptr;
    }
}
//-------------------------------------------------------------------------------------
void ButtonFactory::setupButton(
    ObservableButton& button,
    std::shared_ptr<MenuButtonObserver> observer,
    const sf::Font& font,
    const std::string& texturePath,
    const sf::Color& fallbackColor) {

    // Add observer for event handling
    button.addObserver(observer);

    // Set font
    button.setFont(font);

    // Try to load texture, fallback to solid color
    try {
        auto& texture = AppContext::instance().getTexture(texturePath);
        button.setButtonImage(&texture);
        button.setTextColor(sf::Color::Transparent);
    }
    catch (...) {
        std::cout << "Factory: Using fallback color for button '" << button.getId() << "'" << std::endl;
        button.setBackgroundColor(fallbackColor);
        button.setTextColor(sf::Color::White);
    }
}
//-------------------------------------------------------------------------------------