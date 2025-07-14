#pragma once

#include "IScreen.h"
#include <SFML/Graphics.hpp>

class LoadingScreen : public IScreen {
public:
    LoadingScreen();

    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Font m_font;
    sf::Text m_loadingText;
    sf::Clock m_clock;
    float m_progress = 0.0f;
    bool m_finished = false;

    // Background image
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
};