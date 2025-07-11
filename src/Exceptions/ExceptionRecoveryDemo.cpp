#include "Exceptions/ExceptionRecoverySystem.h"
#include "Exceptions/GameCheckpoint.h"
#include "Exceptions/ResourceRetry.h"
#include "Exceptions/UserFriendlyErrors.h"
#include "Exceptions/RecoveryStrategyFactory.h"
#include "Exceptions/Logger.h"
#include "GameSession.h"
#include "ResourceManager.h"
#include <iostream>
#include <format>
#include <thread>
#include <SFML/Graphics.hpp>

// Global variables for demo purposes
GameSession* g_currentSession = nullptr;
bool g_gameRunning = true;

// Forward declarations for callback functions
void reloadCurrentLevel() {
    if (g_currentSession) {
        g_currentSession->reloadCurrentLevel();
    }
}

void goToMainMenu() {
    std::cout << "Going to main menu (simulation)\n";
    // In a real implementation, we would navigate to the main menu
}

namespace GameExceptions {

/**
 * @brief Demonstration of the exception recovery system
 */
class ExceptionRecoveryDemo {
public:
    static void initialize(GameSession* gameSession) {
        // Store the game session in the global variable for recovery actions
        g_currentSession = gameSession;
        
        // Initialize logger
        auto& logger = getLogger();
        logger.addTarget(std::make_unique<ConsoleLogTarget>());
        logger.setMinLevel(LogLevel::Debug);
        
        logger.info("Exception Recovery System Demo initializing...");
        
        // Initialize the exception recovery system
        auto& recoverySystem = ExceptionRecoverySystem::getInstance();
        recoverySystem.enableTelemetry(true);
        
        // Create checkpoint manager
        auto& checkpointManager = AutoCheckpointManager::getInstance();
        checkpointManager.initialize(gameSession);
        checkpointManager.enableIntervalCheckpoints(60.0f); // Create checkpoints every 60 seconds
        
        // Initialize resource retry manager
        auto& resourceRetryManager = ResourceRetryManager::getInstance();
        
        // Register alternative paths
        resourceRetryManager.registerAlternativePath("textures/player.png", "resources/textures/player.png");
        resourceRetryManager.registerAlternativePath("sounds/explosion.wav", "resources/sounds/explosion.wav");
        
        // Register alternative formats
        resourceRetryManager.registerAlternativeFormat(".png", ".jpg");
        resourceRetryManager.registerAlternativeFormat(".wav", ".ogg");
        
        // Initialize default recovery strategies
        RecoveryStrategyFactory::initializeDefaultRecoverySystem(gameSession);
        
        // Initialize user-friendly error messages
        auto& errorManager = UserFriendlyErrorManager::getInstance();
        
        logger.info("Exception Recovery System Demo initialized");
    }
    
    static void update(float deltaTime) {
        // Update automatic checkpoint manager
        AutoCheckpointManager::getInstance().update(deltaTime);
    }
    
    static void createDemoCheckpoint(const std::string& name, GameSession* gameSession) {
        if (!gameSession) {
            getLogger().error("Cannot create demo checkpoint: GameSession is null");
            return;
        }
        
        getLogger().info(std::format("Creating demo checkpoint: {}", name));
        
        // Create a checkpoint
        auto checkpoint = GameCheckpoint::create(name, gameSession);
        if (checkpoint) {
            getLogger().info("Checkpoint created successfully");
        } else {
            getLogger().error("Failed to create checkpoint");
        }
    }
    
    static void demonstrateResourceRetry() {
        getLogger().info("Demonstrating resource retry mechanism");
        
        // Create a texture manager with retry capabilities
        TextureManager textureManager;
        textureManager.setMaxRetries(3);
        
        // Register a fallback for missing textures
        textureManager.registerFallback("missing_texture.png", "placeholder.png");
        textureManager.registerFallback("*.png", "placeholder.png");
        
        try {
            // Try to load a texture that might not exist
            getLogger().info("Attempting to load texture: nonexistent.png");
            sf::Texture& texture = textureManager.getResource("nonexistent.png");
            getLogger().info("Texture loaded successfully (fallback used)");
        }
        catch (const GameExceptions::ResourceException& ex) {
            getLogger().error(std::format("Resource loading failed: {}", ex.what()));
            
            // Show a user-friendly error message
            auto& errorDialog = ErrorDialog::getInstance();
            errorDialog.showError(ex);
        }
    }
    
    static void demonstrateCheckpointRecovery(GameSession* gameSession) {
        if (!gameSession) {
            getLogger().error("Cannot demonstrate checkpoint recovery: GameSession is null");
            return;
        }
        
        getLogger().info("Demonstrating checkpoint recovery");
        
        // Create a checkpoint
        auto checkpoint = GameCheckpoint::create("RecoveryDemo", gameSession);
        if (!checkpoint) {
            getLogger().error("Failed to create checkpoint for demo");
            return;
        }
        
        getLogger().info("Checkpoint created, simulating error condition...");
        
        // Sleep briefly to simulate game progress
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        try {
            // Simulate a critical error
            throw GameExceptions::Exception("Simulated critical error", 
                GameExceptions::Exception::Severity::Critical, 999);
        }
        catch (const GameExceptions::Exception& ex) {
            getLogger().error(std::format("Critical error occurred: {}", ex.what()));
            
            // Handle the exception with the recovery system
            auto& recoverySystem = ExceptionRecoverySystem::getInstance();
            auto result = recoverySystem.handleException(ex);
            
            if (result == RecoveryResult::Success || result == RecoveryResult::PartialSuccess) {
                getLogger().info("Recovery successful!");
            } else {
                getLogger().error("Recovery failed or not attempted");
                
                // Show user-friendly error
                auto& errorDialog = ErrorDialog::getInstance();
                errorDialog.showError(ex);
            }
        }
    }
    
    static void demonstrateGracefulDegradation() {
        getLogger().info("Demonstrating graceful degradation");
        
        try {
            // Simulate a feature error
            throw GameExceptions::Exception("Simulated feature failure in advanced rendering", 
                GameExceptions::Exception::Severity::Error, 301);
        }
        catch (const GameExceptions::Exception& ex) {
            getLogger().error(std::format("Feature error occurred: {}", ex.what()));
            
            // Create a degraded mode strategy specifically for this feature
            auto degradedStrategy = RecoveryStrategyFactory::createDegradedModeStrategy(
                "AdvancedRendering",
                []() {
                    getLogger().info("Disabling advanced rendering effects");
                    // In a real implementation, we would disable advanced effects
                    return true;
                }
            );
            
            // Apply the strategy directly to demonstrate
            GameExceptions::ExceptionInfo exInfo(ex);
            exInfo.component = "AdvancedRendering";
            auto result = degradedStrategy->apply(exInfo);
            
            if (result == RecoveryResult::Success || result == RecoveryResult::PartialSuccess) {
                getLogger().info("Successfully entered degraded mode for AdvancedRendering");
            } else {
                getLogger().error("Failed to enter degraded mode");
            }
        }
    }
    
    static void demonstrateExceptionTelemetry() {
        getLogger().info("Demonstrating exception telemetry");
        
        auto& telemetry = ExceptionRecoverySystem::getInstance().getTelemetry();
        
        // Generate some sample exceptions for telemetry
        try {
            throw GameExceptions::ResourceNotFoundException("sample1.png");
        } catch (const std::exception& ex) {
            ExceptionInfo info(ex, "ResourceSystem", "LoadTexture");
            telemetry.recordException(info);
        }
        
        try {
            throw GameExceptions::ResourceLoadException("sample2.wav", "Unsupported format");
        } catch (const std::exception& ex) {
            ExceptionInfo info(ex, "AudioSystem", "LoadSound");
            telemetry.recordException(info);
        }
        
        try {
            throw std::runtime_error("Network timeout");
        } catch (const std::exception& ex) {
            ExceptionInfo info(ex, "NetworkSystem", "Connect");
            telemetry.recordException(info);
        }
        
        // Display telemetry statistics
        getLogger().info("Exception telemetry statistics:");
        
        auto byComponent = telemetry.getExceptionsByComponent();
        for (const auto& [component, count] : byComponent) {
            getLogger().info(std::format("  Component {}: {} exceptions", component, count));
        }
        
        auto topExceptions = telemetry.getTopExceptions(3);
        getLogger().info("Top exceptions:");
        for (const auto& [message, count] : topExceptions) {
            getLogger().info(std::format("  {}: {} occurrences", message, count));
        }
        
        // Save telemetry to file
        if (telemetry.saveTelemetryData("exception_telemetry.txt")) {
            getLogger().info("Telemetry data saved to exception_telemetry.txt");
        } else {
            getLogger().error("Failed to save telemetry data");
        }
    }
    
    static void runDemo(GameSession* gameSession) {
        getLogger().info("Starting Exception Recovery System Demo");
        
        // Initialize the system
        initialize(gameSession);
        
        // Create an initial checkpoint
        createDemoCheckpoint("DemoStart", gameSession);
        
        // Demonstrate resource retry
        demonstrateResourceRetry();
        
        // Sleep briefly between demos
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Demonstrate checkpoint recovery
        demonstrateCheckpointRecovery(gameSession);
        
        // Sleep briefly between demos
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Demonstrate graceful degradation
        demonstrateGracefulDegradation();
        
        // Sleep briefly between demos
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Demonstrate telemetry
        demonstrateExceptionTelemetry();
        
        getLogger().info("Exception Recovery System Demo completed");
    }
};

} // namespace GameExceptions