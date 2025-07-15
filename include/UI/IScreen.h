#pragma once

#include <SFML/Graphics.hpp>

class IScreen {
public:
    virtual ~IScreen() = default;

    virtual void handleEvents(sf::RenderWindow& window) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};
