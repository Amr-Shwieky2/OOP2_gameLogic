#include "../Config/ScreenTypes.h"
#include <Application/AppContext.h>
#include "../../include/Screens/LoadingScreen.h"
#include "../Core/AudioManager.h"

LoadingScreen::LoadingScreen() {
    try {
        m_backgroundTexture = AppContext::instance().getTexture("LoadingScreen.png");
        m_backgroundSprite.setTexture(m_backgroundTexture);

        sf::Vector2u textureSize = m_backgroundTexture.getSize();
        sf::Vector2f targetSize(1400.0f, 800.0f); // Window size

        float scaleX = targetSize.x / textureSize.x;
        float scaleY = targetSize.y / textureSize.y;
        m_backgroundSprite.setScale(scaleX, scaleY);

    }
    catch (...) {
        // If image fails to load, create a simple colored background
        m_backgroundTexture.create(800, 600);
        sf::Uint8* pixels = new sf::Uint8[800 * 600 * 4];
        for (int i = 0; i < 800 * 600 * 4; i += 4) {
            pixels[i] = 20;     // R
            pixels[i + 1] = 30; // G
            pixels[i + 2] = 50; // B
            pixels[i + 3] = 255; // A
        }
        m_backgroundTexture.update(pixels);
        m_backgroundSprite.setTexture(m_backgroundTexture);
        delete[] pixels;
    }
    AudioManager::instance().playMusic("loading_music", true);
}

void LoadingScreen::handleEvents(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void LoadingScreen::update(float deltaTime) {
    m_progress += deltaTime * 0.5f; // Loading speed

    if (m_progress >= 1.0f && !m_finished) {
        m_finished = true;
        AppContext::instance().screenManager().changeScreen(ScreenType::MENU);
    }
}

void LoadingScreen::render(sf::RenderWindow& window) {
    // Draw background first
    window.draw(m_backgroundSprite);

    // Draw loading text
    window.draw(m_loadingText);

    // Progress bar
    sf::RectangleShape progressBar(sf::Vector2f(400 * m_progress, 20));
    progressBar.setPosition(500, 700);
    progressBar.setFillColor(sf::Color(245, 245, 220));
    window.draw(progressBar);

    // Progress bar frame
    sf::RectangleShape progressFrame(sf::Vector2f(400, 20));
    progressFrame.setPosition(500, 700);
    progressFrame.setFillColor(sf::Color::Transparent);
    progressFrame.setOutlineThickness(2);
    progressFrame.setOutlineColor(sf::Color::White);
    window.draw(progressFrame);
}
