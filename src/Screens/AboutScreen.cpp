#include "../../include/Screens/AboutScreen.h"
#include "../Config/ScreenTypes.h"
#include <iostream>
#include "AudioManager.h"
#include <Application/AppContext.h>

AboutScreen::AboutScreen() {
    try {
        m_backgroundTexture = AppContext::instance().getTexture("About_UsScreen.png");
        m_backgroundSprite.setTexture(m_backgroundTexture);

        sf::Vector2u textureSize = m_backgroundTexture.getSize();
        if (textureSize.x > 0 && textureSize.y > 0) {
            sf::Vector2f targetSize(1400.0f, 800.0f);
            float scaleX = targetSize.x / textureSize.x;
            float scaleY = targetSize.y / textureSize.y;
            m_backgroundSprite.setScale(scaleX, scaleY);
        }

        std::cout << "About screen image loaded successfully: AboutScreen.png" << std::endl;
    }
    catch (...) {
        std::cout << "Error: Could not load AboutScreen.png!" << std::endl;
    }
}

void AboutScreen::handleEvents(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                AppContext::instance().screenManager().changeScreen(ScreenType::MENU);
            }
        }
    }
}

void AboutScreen::update(float deltaTime) {}

void AboutScreen::render(sf::RenderWindow& window) {
    window.draw(m_backgroundSprite);
}