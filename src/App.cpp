#include "App.h"
#include "Exceptions/Logger.h"
#include "Exceptions/GameExceptions.h"
#include "Exceptions/ExceptionRecoverySystem.h"
#include "Exceptions/GameCheckpoint.h"
#include "Exceptions/UserFriendlyErrors.h"
#include "Exceptions/RecoveryStrategyFactory.h"
#include <iostream>
#include <format>

// Forward declaration
namespace GameExceptions {
    class ExceptionRecoveryDemo;
}

App::App() {
    // Initialize logger with both console and file output
    auto& logger = GameExceptions::getLogger();
    logger.addTarget(std::make_unique<GameExceptions::ConsoleLogTarget>());
    logger.addTarget(std::make_unique<GameExceptions::FileLogTarget>("game_log.txt"));
    logger.info("Game starting");
    
    // Create window
    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(1400, 800),
        "Desert Ball",
        sf::Style::Default  // Enables minimize, maximize, and close buttons
    );

    m_window->setFramerateLimit(60);
    
    try {
        // Initialize the exception recovery system
        initializeExceptionRecoverySystem();
        
        // Initialize game screen
        m_screen = std::make_unique<GameplayScreen>();
        logger.info("Game screen initialized successfully");
        
        // Create initial game checkpoint
        if (auto gameSession = getGameSession()) {
            // Initialize automatic checkpoint manager
            auto& checkpointManager = GameExceptions::AutoCheckpointManager::getInstance();
            checkpointManager.initialize(gameSession);
            checkpointManager.createLevelStartCheckpoint();
            checkpointManager.enableIntervalCheckpoints(60.0f); // Every minute
            
            logger.info("Automatic checkpoint system initialized");
        }
    }
    catch (const std::exception& ex) {
        logger.logException(ex, GameExceptions::LogLevel::Critical);
        
        // Try to recover from initialization error
        auto& recoverySystem = GameExceptions::ExceptionRecoverySystem::getInstance();
        auto result = recoverySystem.handleException(ex);
        
        if (result != GameExceptions::RecoveryResult::Success) {
            // If recovery
