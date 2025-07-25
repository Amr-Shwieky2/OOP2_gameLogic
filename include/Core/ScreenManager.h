#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <functional>
#include "../Config/ScreenTypes.h"
#include "IScreen.h"

// Manages screen transitions and lifecycle
class ScreenManager {
public:
    ScreenManager() = default;
    ~ScreenManager() = default;

    // Register a screen factory function for a given screen type
    void registerScreen(ScreenType type, std::function<std::unique_ptr<IScreen>()> creator);

    // Change to a different screen immediately
    void changeScreen(ScreenType type);

    // Request a screen change to be processed after the current update cycle
    void requestScreenChange(ScreenType type);

    // Handle SFML events for the current screen
    void handleEvents(sf::RenderWindow& window);

    // Update the current screen
    void update(float deltaTime);

    // Render the current screen
    void render(sf::RenderWindow& window);

    // Get the current screen (optional, for debugging)
    IScreen* getCurrentScreen() const { return m_currentScreen.get(); }

private:
    std::unordered_map<ScreenType, std::function<std::unique_ptr<IScreen>()>> m_creators;
    std::unique_ptr<IScreen> m_currentScreen;

    bool m_screenChangeRequested = false;
    ScreenType m_nextScreen = ScreenType::MENU;
};