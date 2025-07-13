#pragma once

#include <SFML/Window.hpp>
#include <unordered_map>

class InputService {
public:
    void update();

    bool isKeyDown(sf::Keyboard::Key key) const;
    bool isKeyPressed(sf::Keyboard::Key key) const;

private:
    std::unordered_map<sf::Keyboard::Key, bool> m_currentState;
    std::unordered_map<sf::Keyboard::Key, bool> m_previousState;
};
