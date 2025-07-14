#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

/**
 * @brief Window management only 
 */
class WindowManager {
public:
    WindowManager();
    ~WindowManager() = default;

    // Window creation and setup
    void createWindow(unsigned int width, unsigned int height, const std::string& title);
    void setFramerateLimit(unsigned int fps);
    void setVerticalSyncEnabled(bool enabled);

    // Window state queries
    bool isWindowOpen() const;
    sf::Vector2u getWindowSize() const;

    // Window control
    void closeWindow();
    void setTitle(const std::string& title);

    // Access to window (for other components)
    sf::RenderWindow& getWindow();
    const sf::RenderWindow& getWindow() const;

private:
    std::unique_ptr<sf::RenderWindow> m_window;

    // Window settings for validation
    unsigned int m_width = 800;
    unsigned int m_height = 600;
    std::string m_title = "Default Window";

    void validateWindowSettings() const;
};
