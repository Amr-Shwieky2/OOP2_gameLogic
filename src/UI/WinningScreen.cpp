#include "WinningScreen.h"
#include <iostream>
#include "../Core/AudioManager.h"
#include <Application/AppContext.h>

WinningScreen::WinningScreen(const std::string& textureFile)
    : m_textureFile(textureFile) {
}

void WinningScreen::show(sf::RenderWindow& window) {
    AudioManager::instance().stopAllSounds();
    AudioManager::instance().playSound("win");

    sf::Texture& texture = AppContext::instance().getTexture(m_textureFile);
    sf::Sprite sprite(texture);

    bool running = true;
    while (running && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                running = false;
                AudioManager::instance().stopSound("win");
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}
