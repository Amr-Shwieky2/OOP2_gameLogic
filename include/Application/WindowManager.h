#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

/**
 * @brief Responsible for managing the main SFML render window.
 *
 * This class handles the creation, configuration, and control of the SFML window.
 * It exposes safe accessors for other components (such as GameLoop and ScreenManager)
 * and ensures validation of window parameters.
 *
 * Follows the Single Responsibility Principle (SRP): only handles window lifecycle and settings.
 */
class WindowManager {
public:
    /**
     * @brief Constructs the WindowManager with default settings (800x600 window).
     */
    WindowManager();

    /**
     * @brief Default destructor.
     */
    ~WindowManager() = default;

    /**
     * @brief Creates the SFML render window with given dimensions and title.
     *
     * Validates window parameters before creating.
     *
     * @param width Window width in pixels.
     * @param height Window height in pixels.
     * @param title Title to display in the window's title bar.
     */
    void createWindow(unsigned int width, unsigned int height, const std::string& title);

    /**
     * @brief Sets the maximum number of frames per second (FPS).
     *
     * @param fps Frame limit (e.g., 60).
     */
    void setFramerateLimit(unsigned int fps);

    /**
     * @brief Enables or disables vertical synchronization.
     *
     * @param enabled True to enable vsync, false to disable.
     */
    void setVerticalSyncEnabled(bool enabled);

    /**
     * @brief Checks if the window has been created and is currently open.
     *
     * @return true if the window is open, false otherwise.
     */
    bool isWindowOpen() const;

    /**
     * @brief Returns the current size of the window in pixels.
     *
     * @return A vector (width, height).
     */
    sf::Vector2u getWindowSize() const;

    /**
     * @brief Closes the window safely.
     */
    void closeWindow();

    /**
     * @brief Changes the window's title at runtime.
     *
     * @param title New title string.
     */
    void setTitle(const std::string& title);

    /**
     * @brief Returns a reference to the SFML render window (mutable).
     *
     * Throws if the window hasn't been created yet.
     *
     * @return Reference to sf::RenderWindow.
     */
    sf::RenderWindow& getWindow();

    /**
     * @brief Returns a const reference to the SFML render window (read-only).
     *
     * Throws if the window hasn't been created yet.
     *
     * @return Const reference to sf::RenderWindow.
     */
    const sf::RenderWindow& getWindow() const;

private:
    std::unique_ptr<sf::RenderWindow> m_window; ///< Internal pointer to the SFML window.

    // Window settings (used for validation or fallback)
    unsigned int m_width = 800;   ///< Window width in pixels (default: 800).
    unsigned int m_height = 600;  ///< Window height in pixels (default: 600).
    std::string m_title = "Default Window"; ///< Initial window title.

    /**
     * @brief Validates window settings before creation.
     *
     * Throws if settings are invalid (e.g., zero size or empty title).
     */
    void validateWindowSettings() const;
};
