#pragma once
#include <SFML/Graphics.hpp>
#include "InputService.h"
#include "VoiceInputService.h"
#include "Player.h"

class InputManager {
public:
    InputManager();
    ~InputManager();

    void initialize(int levelIndex);
    void handleEvents(sf::RenderWindow& window);
    void updatePlayer(Player& player);
    void shutdown();

private:
    InputService m_keyboardInput;
    VoiceInputService m_voiceInput;
    bool m_useVoiceInput = false;

    void handleKeyboardInput(Player& player);
    void handleVoiceInput(Player& player);
};