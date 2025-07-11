#include "Exceptions/RecoveryStrategyFactory.h"
#include "Exceptions/GameCheckpoint.h"
#include "Exceptions/UserFriendlyErrors.h"
#include "GameSession.h"
#include <format>

namespace GameExceptions {

std::shared_ptr<RecoveryStrategy> RecoveryStrategyFactory::createCheckpointStrategy(
    GameSession* gameSession, 
    const std::string& checkpointName)
{
    // Create a checkpoint
    auto checkpoint = GameCheckpoint::create(checkpointName, gameSession);
    if (!checkpoint) {
        getLogger().error("Failed to create checkpoint for recovery strategy");
        return nullptr;
    }
    
    // Create a checkpoint recovery strategy
    return std::make_shared<CheckpointRecoveryStrategy>(checkpoint);
}

std::shared_ptr<RecoveryStrategy> RecoveryStrategyFactory::createLevelReloadStrategy(GameSession* gameSession) {
    // Create a function that reloads the current level
    auto reloadFunc = [gameSession]() -> bool {
        if (!gameSession) {
            getLogger().error("Cannot reload level: GameSession is null");
            return false;
        }
        
        try {
            std::string currentLevel = gameSession->getCurrentLevelName();
            getLogger().info(std::format("Reloading level: {}", currentLevel));
            
            bool success = gameSession->reloadCurrentLevel();
            if (success) {
                getLogger().info("Level reload successful");
                return true;
            } else {
                getLogger().error("Level reload failed");
                return false;
            }
        }
        catch (const std::exception& ex) {
            getLogger().error(std::format("Exception during level reload: {}", ex.what()));
            return false;
        }
    };
    
    // Create a fallback strategy that executes the reload function
    return std::make_shared<FallbackRecoveryStrategy>(reloadFunc, "LevelSystem");
}

std::shared_ptr<RecoveryStrategy> RecoveryStrategyFactory::createFallbackStrategy(
    const std::string& componentName,
    std::function<bool()> fallbackFunction)
{
    return std::make_shared<FallbackRecoveryStrategy>(fallbackFunction, componentName);
}

std::shared_ptr<RecoveryStrategy> RecoveryStrategyFactory::createDegradedModeStrategy(
    const std::string& featureName,
    std::function<bool()> degradeFunction)
{
    return std::make_shared<DegradedModeStrategy>(degradeFunction, featureName);
}

std::shared_ptr<UserPromptStrategy> RecoveryStrategyFactory::createUserPromptStrategy(
    const std::vector<std::string>& options,
    std::function<int(const ExceptionInfo&, const std::vector<std::string>&)> promptFunction)
{
    return std::make_shared<UserPromptStrategy>(options, promptFunction);
}

void RecoveryStrategyFactory::initializeDefaultRecoverySystem(GameSession* gameSession) {
    auto& recoverySystem = ExceptionRecoverySystem::getInstance();
    
    // Create and register checkpoint strategy
    if (gameSession) {
        auto checkpointStrategy = createCheckpointStrategy(gameSession, "DefaultCheckpoint");
        if (checkpointStrategy) {
            recoverySystem.registerStrategy(checkpointStrategy);
        }
        
        // Register level reload strategy
        auto levelReloadStrategy = createLevelReloadStrategy(gameSession);
        recoverySystem.registerStrategy(levelReloadStrategy);
    }
    
    // Register common fallback strategies
    
    // Fallback for rendering errors
    auto renderFallbackStrategy = createFallbackStrategy("Rendering", []() {
        getLogger().info("Switching to fallback rendering mode");
        // In a real implementation, we would have code to switch to simpler rendering
        return true;
    });
    recoverySystem.registerStrategy(renderFallbackStrategy);
    
    // Degraded mode for audio errors
    auto audioDegradedStrategy = createDegradedModeStrategy("Audio", []() {
        getLogger().info("Disabling audio due to errors");
        // In a real implementation, we would disable audio
        return true;
    });
    recoverySystem.registerStrategy(audioDegradedStrategy);
    
    // Degraded mode for physics errors
    auto physicsDegradedStrategy = createDegradedModeStrategy("Physics", []() {
        getLogger().info("Switching to simplified physics");
        // In a real implementation, we would simplify physics
        return true;
    });
    recoverySystem.registerStrategy(physicsDegradedStrategy);
    
    // User prompt strategy for critical errors
    auto userPromptStrategy = createUserPromptStrategy(
        {"Reload Level", "Exit to Menu", "Exit Game"},
        [](const ExceptionInfo& exInfo, const std::vector<std::string>& options) -> int {
            // In a real implementation, we would show a UI dialog
            // For now, just log the choices and return a default
            getLogger().info("User prompt for error:");
            getLogger().info(exInfo.message);
            getLogger().info("Options:");
            for (size_t i = 0; i < options.size(); ++i) {
                getLogger().info(std::format("{}. {}", i + 1, options[i]));
            }
            return 0; // Default to first option
        }
    );
    
    // Add actions for each choice
    userPromptStrategy->addAction(0, DefaultRecoveryActions::reloadCurrentLevel);
    userPromptStrategy->addAction(1, DefaultRecoveryActions::goToMainMenu);
    userPromptStrategy->addAction(2, DefaultRecoveryActions::exitGame);
    
    recoverySystem.registerStrategy(userPromptStrategy);
    
    getLogger().info("Default recovery strategies initialized");
}

} // namespace GameExceptions