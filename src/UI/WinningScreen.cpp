#include "WinningScreen.h"
#include <iostream>
#include "../Core/AudioManager.h"

//-------------------------------------------------------------------------------------
WinningScreen::WinningScreen(const std::string& textureFile)
    : m_textureFile(textureFile) {
}
//-------------------------------------------------------------------------------------
void WinningScreen::show(sf::RenderWindow& window) {
    AudioManager::instance().pauseMusic();
    AudioManager::instance().playSound("win");

    sf::Texture texture;
    if (!texture.loadFromFile(m_textureFile)) {
        std::cerr << "[ERROR] Could not load " << m_textureFile << std::endl;
        return;
    }

    sf::Sprite sprite(texture);

    bool running = true;
    while (running && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                running = false;
                AudioManager::instance().stopSound("win");
                AudioManager::instance().playMusic("loading_music", true);
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}
//-------------------------------------------------------------------------------------