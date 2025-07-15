#include "Logger.h"
#include <stdexcept>
#include "GameExceptions.h"
#include <Application/WindowManager.h>
#include <Services/Logger.h>
#include <Services/GameExceptions.h>

//-------------------------------------------------------------------------------------
WindowManager::WindowManager() {}
//-------------------------------------------------------------------------------------
void WindowManager::createWindow(unsigned int width, unsigned int height, const std::string& title) {
    m_width = width;
    m_height = height;
    m_title = title;

    validateWindowSettings();

    try {
        m_window = std::make_unique<sf::RenderWindow>(
            sf::VideoMode(width, height), title
        );

        Logger::log("Window created: " + title + " (" +
            std::to_string(width) + "x" + std::to_string(height) + ")");
    }
    catch (const std::exception& e) {
        Logger::log("Failed to create window: " + std::string(e.what()), LogLevel::Error);
        throw;
    }
}
//-------------------------------------------------------------------------------------
void WindowManager::setFramerateLimit(unsigned int fps) {
    if (!m_window) {
        Logger::log("Cannot set framerate: Window not created", LogLevel::Warning);
        return;
    }
    m_window->setFramerateLimit(fps);
    Logger::log("Framerate limit set to: " + std::to_string(fps));
}
//-------------------------------------------------------------------------------------
void WindowManager::setVerticalSyncEnabled(bool enabled) {
    if (!m_window) {
        Logger::log("Cannot set vsync: Window not created", LogLevel::Warning);
        return;
    }
    m_window->setVerticalSyncEnabled(enabled);
    Logger::log("Vertical sync " + std::string(enabled ? "enabled" : "disabled"));
}
//-------------------------------------------------------------------------------------
bool WindowManager::isWindowOpen() const {
    return m_window && m_window->isOpen();
}
//-------------------------------------------------------------------------------------
sf::Vector2u WindowManager::getWindowSize() const {
    if (!m_window) {
        return sf::Vector2u(m_width, m_height);
    }
    return m_window->getSize();
}
//-------------------------------------------------------------------------------------
void WindowManager::closeWindow() {
    if (m_window && m_window->isOpen()) {
        m_window->close();
        Logger::log("Window closed");
    }
}
//-------------------------------------------------------------------------------------
void WindowManager::setTitle(const std::string& title) {
    m_title = title;
    if (m_window) {
        m_window->setTitle(title);
        Logger::log("Window title changed to: " + title);
    }
}
//-------------------------------------------------------------------------------------
sf::RenderWindow& WindowManager::getWindow() {
    if (!m_window) {
        THROW_WINDOW_EXCEPTION("Window not created - call createWindow() first");
    }
    return *m_window;
}
//-------------------------------------------------------------------------------------
const sf::RenderWindow& WindowManager::getWindow() const {
    if (!m_window) {
        THROW_WINDOW_EXCEPTION("Window not created - call createWindow() first");
    }
    return *m_window;
}
//-------------------------------------------------------------------------------------
void WindowManager::validateWindowSettings() const {
    if (m_width < 100 || m_height < 100) {
        THROW_WINDOW_EXCEPTION("Window size too small (minimum 100x100)");
    }
    if (m_width > 4096 || m_height > 4096) {
        Logger::log("Large window size detected: " +
            std::to_string(m_width) + "x" + std::to_string(m_height),
            LogLevel::Warning);
    }
    if (m_title.empty()) {
        THROW_WINDOW_EXCEPTION("Window title cannot be empty");
    }
}
//-------------------------------------------------------------------------------------