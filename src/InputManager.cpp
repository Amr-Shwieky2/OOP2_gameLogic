#include "InputManager.h"

InputManager::InputManager() = default;

InputManager::~InputManager() {
    shutdown();
}

void InputManager::initialize(int levelIndex) {
    if (levelIndex == 1) {
        m_useVoiceInput = true;
        m_voiceInput.start();
    }
}

void InputManager::handleEvents(sf::RenderWindow& window) {
    m_keyboardInput.update();

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void InputManager::updatePlayer(Player& player) {
    if (m_useVoiceInput) {
        handleVoiceInput(player);
    }
    else {
        handleKeyboardInput(player);
    }

    // Handle shooting (always keyboard)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
        player.shoot(player.getTextureManager());
    }
}

void InputManager::handleKeyboardInput(Player& player) {
    player.handleInput(m_keyboardInput);
}

void InputManager::handleVoiceInput(Player& player) {
    float volume = m_voiceInput.getVolume();
    if (volume > 0.7f) {
        player.jump();
    }
    else if (volume > 0.2f) {
        player.moveForward(volume);
    }
}

void InputManager::shutdown() {
    if (m_useVoiceInput) {
        m_voiceInput.stop();
    }
}