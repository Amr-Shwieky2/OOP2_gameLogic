#include "UITheme.h"

UITheme::Colors UITheme::getDesertTheme() {
    return {
        sf::Color(194, 154, 108, 200),      // background (sand)
        sf::Color(245, 222, 179, 240),      // panel (papyrus)
        sf::Color(139, 90, 43),             // titleText (dark wood)
        sf::Color(101, 67, 33),             // labelText (dark brown)
        sf::Color(178, 34, 34),             // valueText (fire brick)
        sf::Color(139, 90, 43),             // sliderBackground
        sf::Color(255, 165, 0),             // sliderFill (desert orange)
        sf::Color(255, 215, 0),             // sliderHandle (gold)
        sf::Color(245, 222, 179),           // dropdownBackground
        sf::Color(101, 67, 33),             // dropdownText
        sf::Color(255, 215, 0)              // dropdownHighlight
    };
}

UITheme::Colors UITheme::getSpaceTheme() {
    return {
        sf::Color(20, 25, 40, 200),         // background (space blue)
        sf::Color(40, 50, 80, 240),         // panel
        sf::Color::Yellow,                   // titleText
        sf::Color::White,                    // labelText
        sf::Color::Cyan,                     // valueText
        sf::Color(50, 50, 50),              // sliderBackground
        sf::Color(100, 150, 255),           // sliderFill
        sf::Color(200, 200, 200),           // sliderHandle
        sf::Color(70, 70, 70),              // dropdownBackground
        sf::Color::White,                    // dropdownText
        sf::Color(100, 150, 255)            // dropdownHighlight
    };
}

UITheme::Colors UITheme::getDefaultTheme() {
    return getDesertTheme(); // Default to desert theme
}

UITheme::Layout UITheme::getDefaultLayout() {
    return {
        250.0f,                             // startY
        90.0f,                              // spacing
        350.0f,                             // labelX
        560.0f,                             // controlX
        850.0f,                             // valueX
        sf::Vector2f(250.0f, 35.0f),        // sliderSize
        sf::Vector2f(200.0f, 60.0f)         // dropdownSize
    };
}

UITheme::Fonts UITheme::getDefaultFonts() {
    return {
        "arial.ttf",                        
    };
}