#pragma once
#include "../Core/IScreen.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class AboutScreen : public IScreen {
public:
    AboutScreen();
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
};