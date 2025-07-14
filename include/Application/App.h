#pragma once
#include <memory>

#include "WindowManager.h"
#include "GameInitializer.h" 
#include "GameLoop.h"
#include "AppCleanupManager.h"
/**
 * @brief Main application coordinator - follows SRP
 *
 * Responsibility: Coordinate other components only
 * Does NOT handle: Window creation, audio, rendering, cleanup directly
 */
class App {
public:
    App();
    ~App() = default;

    // Main application entry point
    void run();

private:
    // Application lifecycle
    void initialize();
    void cleanup();

    // SRP Components - each handles one responsibility
    std::unique_ptr<WindowManager> m_windowManager;
    std::unique_ptr<GameInitializer> m_initializer;
    std::unique_ptr<GameLoop> m_gameLoop;
    std::unique_ptr<AppCleanupManager> m_cleanupManager;

    // Error handling
    void handleApplicationError(const std::exception& e);
};