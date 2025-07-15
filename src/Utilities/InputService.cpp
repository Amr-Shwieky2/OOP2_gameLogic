#include "InputService.h"

//-------------------------------------------------------------------------------------
void InputService::update() {
    m_previousState = m_currentState;

    for (int k = sf::Keyboard::A; k <= sf::Keyboard::KeyCount; ++k) {
        sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(k);
        m_currentState[key] = sf::Keyboard::isKeyPressed(key);
    }
}
//-------------------------------------------------------------------------------------
bool InputService::isKeyDown(sf::Keyboard::Key key) const {
    auto it = m_currentState.find(key);
    return (it != m_currentState.end()) && it->second;
}
//-------------------------------------------------------------------------------------
bool InputService::isKeyPressed(sf::Keyboard::Key key) const {
    auto curr = m_currentState.find(key);
    auto prev = m_previousState.find(key);

    bool isDownNow = curr != m_currentState.end() && curr->second;
    bool wasDownBefore = prev != m_previousState.end() && prev->second;

    return isDownNow && !wasDownBefore;
}
//-------------------------------------------------------------------------------------