#pragma once
#include <SFML/Graphics.hpp>

class WindowManager;

/**
 * @brief Handles the main game loop execution.
 *
 * The GameLoop class is responsible for continuously updating and rendering the game
 * while the window remains open. It also tracks frame timing and manages exceptions gracefully.
 *
 * Responsibilities:
 * - Process events and user input.
 * - Update game logic with accurate timing (deltaTime).
 * - Render current screen/frame.
 * - Handle runtime errors without crashing the application.
 */
class GameLoop {
public:
    /**
     * @brief Constructs the GameLoop with a reference to the WindowManager.
     *
     * @param windowManager Manages the SFML window and its lifecycle.
     */
    explicit GameLoop(WindowManager& windowManager);

    /**
     * @brief Default destructor.
     */
    ~GameLoop() = default;

    /**
     * @brief Starts the main loop and keeps running until the window is closed.
     *
     * The loop processes input, updates logic, renders frames, and handles errors.
     */
    void run();

private:
    // === Loop internals ===

    /**
     * @brief Processes a single game frame: update + render.
     */
    void processFrame();

    /**
     * @brief Updates game logic and handles input events.
     *
     * @param deltaTime Time elapsed since the last frame (in seconds).
     */
    void updateGame(float deltaTime);

    /**
     * @brief Renders the current game state to the screen.
     */
    void renderGame();

    /**
     * @brief Calculates time difference between frames.
     *
     * Caps the result to avoid jumps when performance drops.
     * @return Delta time in seconds.
     */
    float calculateDeltaTime();

    // === Members ===
    WindowManager& m_windowManager;  ///< Reference to the window manager.
    sf::Clock m_clock;               ///< Clock used for measuring frame time.

    // Frame rate limiter to avoid unstable delta time
    static constexpr float MAX_DELTA_TIME = 1.0f / 30.0f; ///< Maximum allowed delta time (30 FPS floor).
};
