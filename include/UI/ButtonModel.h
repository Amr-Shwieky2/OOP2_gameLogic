#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class ButtonModel {
public:
    sf::Vector2f position;
    sf::Vector2f size;
    std::string text;

    sf::Color backgroundColor = sf::Color(100, 100, 100, 255);
    sf::Color textColor = sf::Color::White;

    const sf::Texture* texture = nullptr;
    const sf::Font* font = nullptr;

    ButtonModel() = default;

    sf::FloatRect getBounds() const {
        return sf::FloatRect(position, size);
    }
};
