#pragma once

#include "Exceptions/ExceptionRecoverySystem.h"
#include "Exceptions/GameCheckpoint.h"
#include <memory>
#include <string>
#include <functional>

namespace GameExceptions {

/**
 * @brief Factory class for creating common recovery strategies
 */
class RecoveryStrategyFactory {
public:
    /**
     * @brief Create a checkpoint-based recovery strategy
     * 
     * @param gameSession The game session
     * @param checkpointName Name for the checkpoint
     * @return std::shared_ptr<RecoveryStrategy> The created strategy
     */
    static std::shared_ptr<RecoveryStrategy> createCheckpointStrategy(
        GameSession* gameSession, 
        const std::string& checkpointName = "AutoCheckpoint");
    
    /**
     * @brief Create a retry strategy for resource loading
     * 
     * @param resourcePath The resource path to load
     * @param loadFunction The function to load the resource
     * @param maxRetries Maximum number of retries (default: 3)
     * @return std::shared_ptr<RecoveryStrategy> The created strategy
     */
    template<typename T>
    static std::shared_ptr<RetryRecoveryStrategy> createResourceLoadRetryStrategy(
        const std::string& resourcePath,
        std::function<T(const std::string&)> loadFunction,
        int maxRetries = 3)
    {
        // Create a retry function
        auto operation = [=]() -> bool {
            try {
                T resource = loadFunction(resourcePath);
                return true; // Success
            }
            catch (const std::exception& ex) {
                getLogger().warning(std::format("Failed to load resource {}: {}", resourcePath, ex.what()));
                return false; // Failed
            }
        };
        
        // Create retry policy
        RetryPolicy policy(maxRetries, std::chrono::milliseconds(500));
        policy.setExponentialBackoff(true, 2.0f);
        
        // Create retry strategy
        return std::make_shared<RetryRecoveryStrategy>(
            operation,
            policy,
            "Load " + resourcePath
        );
    }
    
    /**
     * @brief Create a strategy for reverting a level to the last checkpoint
     * 
     * @param gameSession The game session
     * @return std::shared_ptr<RecoveryStrategy> The created strategy
     */
    static std::shared_ptr<RecoveryStrategy> createLevelReloadStrategy(GameSession* gameSession);
    
    /**
     * @brief Create a strategy for falling back to a simpler implementation
     * 
     * @param componentName The component name
     * @param fallbackFunction The fallback function to execute
     * @return std::shared_ptr<RecoveryStrategy> The created strategy
     */
    static std::shared_ptr<RecoveryStrategy> createFallbackStrategy(
        const std::string& componentName,
        std::function<bool()> fallbackFunction);
    
    /**
     * @brief Create a strategy for graceful degradation of a feature
     * 
     * @param featureName The feature name
     * @param degradeFunction The function to degrade the feature
     * @return std::shared_ptr<RecoveryStrategy> The created strategy
     */
    static std::shared_ptr<RecoveryStrategy> createDegradedModeStrategy(
        const std::string& featureName,
        std::function<bool()> degradeFunction);
    
    /**
     * @brief Create a user prompt strategy with choices
     * 
     * @param options The list of options to present to the user
     * @param promptFunction The function to show the prompt and get user choice
     * @return std::shared_ptr<RecoveryStrategy> The created strategy
     */
    static std::shared_ptr<UserPromptStrategy> createUserPromptStrategy(
        const std::vector<std::string>& options,
        std::function<int(const ExceptionInfo&, const std::vector<std::string>&)> promptFunction);
    
    /**
     * @brief Initialize the recovery system with a standard set of strategies
     * 
     * @param gameSession The game session
     */
    static void initializeDefaultRecoverySystem(GameSession* gameSession);
};

} // namespace GameExceptions