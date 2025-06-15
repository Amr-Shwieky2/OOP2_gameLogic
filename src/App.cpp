#include "App.h"
#include <iostream>

App::App() {
    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(1400, 800),
        "Red Ball Clone",
        sf::Style::Default  // Enables minimize, maximize, and close buttons
    );

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

        sf::Event event;
        while (m_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window->close();
            }
            else if (event.type == sf::Event::Resized) {
                // Automatically adjust the view to match new window size
                sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                m_window->setView(sf::View(visibleArea));
            }
        }

        m_screen->handleEvents(*m_window);
        m_screen->update(dt);

        m_window->clear();
        m_screen->render(*m_window);
        m_window->display();
    }
}
