// App.cpp
#include "App.h"
#include <iostream>

App::App() {
    m_window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1400, 800), "Red Ball Clone");
    m_window->setFramerateLimit(60);

    m_screen = std::make_unique<GameplayScreen>();
}

void App::run() {
    try {
        mainLoop();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in run(): " << e.what() << std::endl;
    }
}

void App::mainLoop() {
    sf::Clock clock;
    while (m_window->isOpen()) {
        float dt = clock.restart().asSeconds();

        m_screen->handleEvents(*m_window);
        m_screen->update(dt);

        m_window->clear();
        m_screen->render(*m_window);
        m_window->display();
    }
}
