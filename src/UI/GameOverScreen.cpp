#include "GameOverScreen.h"
#include "../Core/AudioManager.h"
#include <Application/AppContext.h>
#include <iostream>

GameOverScreen::GameOverScreen(const std::string& textureFile)
    : m_textureFile(textureFile) {}

void GameOverScreen::show(sf::RenderWindow& window) {
    AudioManager::instance().pauseMusic();
    AudioManager::instance().playSound("gameover");

    sf::Texture& texture = AppContext::instance().getTexture(m_textureFile);
    sf::Sprite sprite(texture);
    bool running = true;

    while (running && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                running = false;
                AudioManager::instance().stopSound("gameover");
                AudioManager::instance().playMusic("loading_music", true);
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}
