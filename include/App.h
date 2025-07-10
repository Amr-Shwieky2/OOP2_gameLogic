// App.h
#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "IScreen.h"
#include "GameplayScreen.h"

// App.h
class App {
public:
    App();
    void run();

private:
    void mainLoop();
    void displayErrorMessage(const std::string& message);

    std::unique_ptr<sf::RenderWindow> m_window;
    std::unique_ptr<IScreen> m_screen;

    bool m_isFullscreen = false;
};