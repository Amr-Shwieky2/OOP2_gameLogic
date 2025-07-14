#include <Logger.h>
#include <Application/App.h>
#include <Services/Logger.h>

App::App()
    : m_windowManager(std::make_unique<WindowManager>())
    , m_initializer(std::make_unique<GameInitializer>())
    , m_cleanupManager(std::make_unique<AppCleanupManager>()) {

    // GameLoop needs WindowManager, so create it after WindowManager
    // This will be done in initialize() after window is created

    Logger::log("App created with SRP components");
}

void App::run() {
    Logger::log("Starting Desert Ball game...");

    try {
        initialize();

        // Create game loop after initialization
        m_gameLoop = std::make_unique<GameLoop>(*m_windowManager);

        // Run the main game loop
        m_gameLoop->run();

        cleanup();
    }
    catch (const std::exception& e) {
        handleApplicationError(e);
    }

    Logger::log("Game ended.");
}

void App::initialize() {
    Logger::log("Initializing application...");

    // Step 1: Create and setup window
    m_windowManager->createWindow(1400, 800, "Desert Ball");
    m_windowManager->setFramerateLimit(60);
    m_windowManager->setVerticalSyncEnabled(false);

    // Step 2: Initialize all game systems
    m_initializer->initializeAllSystems();

    Logger::log("Application initialization completed");
}

void App::cleanup() {
    Logger::log("Cleaning up application...");

    // Cleanup in reverse order of creation
    try {
        // Stop game loop first
        m_gameLoop.reset();

        // Perform cleanup operations
        m_cleanupManager->performCleanup();

        // Close window last
        if (m_windowManager) {
            m_windowManager->closeWindow();
        }

        Logger::log("Application cleanup completed successfully");
    }
    catch (const std::exception& e) {
        Logger::log("Error during cleanup: " + std::string(e.what()), LogLevel::Warning);
        // Continue with cleanup - don't let cleanup errors crash the app
    }
}

void App::handleApplicationError(const std::exception& e) {
    Logger::log("Critical application error: " + std::string(e.what()), LogLevel::Error);

    // Attempt emergency cleanup
    try {
        if (m_cleanupManager) {
            m_cleanupManager->performCleanup();
        }
    }
    catch (...) {
        Logger::log("Emergency cleanup failed", LogLevel::Error);
    }

    Logger::log("Application terminated due to error");
}