// main.cpp
#include "App.h"
#include "Exceptions/Logger.h"
#include "Exceptions/GameExceptions.h"
#include <iostream>
#include <filesystem>

int main() {
    // Create logs directory if it doesn't exist
    try {
        std::filesystem::path logDir = "logs";
        if (!std::filesystem::exists(logDir)) {
            std::filesystem::create_directory(logDir);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to create logs directory: " << e.what() << std::endl;
        // Continue anyway
    }
    
    // Initialize exception system before any other code
    try {
        // Run the application
        App app;
        app.run();
    }
    catch (const GameExceptions::Exception& e) {
        GameExceptions::getLogger().logException(e, GameExceptions::LogLevel::Critical);
        std::cerr << "[FATAL] Unhandled exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        GameExceptions::getLogger().error(std::string("Unhandled standard exception: ") + e.what());
        std::cerr << "[FATAL] Unhandled exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        GameExceptions::getLogger().critical("Unknown exception occurred");
        std::cerr << "[FATAL] Unknown exception occurred." << std::endl;
        return EXIT_FAILURE;
    }
    
    GameExceptions::getLogger().info("Game exited successfully");
    return EXIT_SUCCESS;
}
