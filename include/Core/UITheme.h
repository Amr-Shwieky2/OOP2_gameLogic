#pragma once
#include <SFML/Graphics.hpp>

class UITheme {
public:
    struct Colors {
        sf::Color background;
        sf::Color panel;
        sf::Color titleText;
        sf::Color labelText;
        sf::Color valueText;
        sf::Color sliderBackground;
        sf::Color sliderFill;
        sf::Color sliderHandle;
        sf::Color dropdownBackground;
        sf::Color dropdownText;
        sf::Color dropdownHighlight;
    };

    struct Fonts {
        std::string mainFont;
        std::string titleFont;
    };

    struct Layout {
        float startY;
        float spacing;
        float labelX;
        float controlX;
        float valueX;
        sf::Vector2f sliderSize;
        sf::Vector2f dropdownSize;
    };

    static Colors getDesertTheme();
    static Colors getSpaceTheme();
    static Colors getDefaultTheme();

    static Layout getDefaultLayout();
    static Fonts getDefaultFonts();
};
