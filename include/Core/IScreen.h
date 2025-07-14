#pragma once
#include <SFML/Graphics.hpp>

/**
 * @brief Interface for all game screens (menu, gameplay, settings, etc.)
 *
 * Each screen handles its own input, logic updates, and rendering.
 * Follows State Pattern - different screens = different game states.
 */
class IScreen {
public:
    virtual ~IScreen() = default;

    // Handle user input (keyboard, mouse, window events)
    virtual void handleEvents(sf::RenderWindow& window) = 0;

    // Update game logic each frame (animations, movement, etc.)
    virtual void update(float deltaTime) = 0;

    // Draw everything to screen (sprites, text, UI)
    virtual void render(sf::RenderWindow& window) = 0;

    // Optional lifecycle hooks
    virtual void onEnter() {}
    virtual void onExit() {}
};