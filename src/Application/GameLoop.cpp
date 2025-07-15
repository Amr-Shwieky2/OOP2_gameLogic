#include "../Application/AppContext.h"
#include <Application/GameLoop.h>
#include "../Application/GameLoop.h"  
#include "../Application/WindowManager.h"
#include <Services/Logger.h>
#include <Application/AppContext.h>

//-------------------------------------------------------------------------------------
GameLoop::GameLoop(WindowManager& windowManager)
    : m_windowManager(windowManager) {
    Logger::log("GameLoop created");
}
//-------------------------------------------------------------------------------------
void GameLoop::run() {
    Logger::log("Starting main game loop...");

    try {
        while (m_windowManager.isWindowOpen()) {
            processFrame();
        }
    }
    catch (const std::exception& e) {
        Logger::log("Game loop error: " + std::string(e.what()), LogLevel::Error);
        throw;
    }
    Logger::log("Game loop ended");
}
//-------------------------------------------------------------------------------------
void GameLoop::processFrame() {
    float deltaTime = calculateDeltaTime();

    // Update game logic
    updateGame(deltaTime);

    // Render frame
    renderGame();
}
//-------------------------------------------------------------------------------------
void GameLoop::updateGame(float deltaTime) {
    try {
        auto& screenManager = AppContext::instance().screenManager();

        // Handle events first
        screenManager.handleEvents(m_windowManager.getWindow());

        // Update current screen
        screenManager.update(deltaTime);
    }
    catch (const std::exception& e) {
        Logger::log("Update error: " + std::string(e.what()), LogLevel::Error);
    }
}
//-------------------------------------------------------------------------------------
void GameLoop::renderGame() {
    try {
        auto& window = m_windowManager.getWindow();
        auto& screenManager = AppContext::instance().screenManager();

        // Clear screen
        window.clear(sf::Color::Black);

        // Render current screen
        screenManager.render(window);

        // Display frame
        window.display();
    }
    catch (const std::exception& e) {
        Logger::log("Render error: " + std::string(e.what()), LogLevel::Error);
    }
}
//-------------------------------------------------------------------------------------
float GameLoop::calculateDeltaTime() {
    float deltaTime = m_clock.restart().asSeconds();
    if (deltaTime > MAX_DELTA_TIME) {
        deltaTime = MAX_DELTA_TIME;
        Logger::log("Delta time capped to prevent frame skip", LogLevel::Warning);
    }
    return deltaTime;
}
//-------------------------------------------------------------------------------------