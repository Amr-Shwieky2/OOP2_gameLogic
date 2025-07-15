#pragma once  
#include "../Core/IScreen.h"  
#include <SFML/Graphics.hpp>
#include <iostream>

class HelpScreen : public IScreen {
public:
    HelpScreen();

    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    // Timer to control how long the help screen is displayed
    float m_elapsedTime = 0.0f;
    const float m_displayDuration = 3.0f; // seconds
    bool m_startRequested = false;
};