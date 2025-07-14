#pragma once
#include <SFML/Graphics.hpp>

class WindowManager;

/**
 * @brief Main game loop only 
 */
class GameLoop {
public:
    explicit GameLoop(WindowManager& windowManager);
    ~GameLoop() = default;

    // Main loop entry point
    void run();

private:
    // Loop components
    void processFrame();
    void updateGame(float deltaTime);
    void renderGame();

    // Frame timing
    float calculateDeltaTime();

    WindowManager& m_windowManager;
    sf::Clock m_clock;

    // Performance tracking
    static constexpr float MAX_DELTA_TIME = 1.0f / 30.0f; // Cap at 30 FPS minimum
};
