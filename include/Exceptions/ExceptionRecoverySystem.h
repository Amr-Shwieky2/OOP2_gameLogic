#pragma once

#include "Exception.h"
#include "Logger.h"
#include "SafeResource.h"
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <optional>
#include <any>
#include <mutex>
#include <atomic>

namespace GameExceptions {

// Forward declarations
class GameSession;
class RecoveryCheckpoint;
class RecoveryStrategy;
class RetryPolicy;

/**
 * @brief Enum for recovery attempt result
 */
enum class RecoveryResult {
    Success,        // Recovery was successful
    Failure,        // Recovery failed
    PartialSuccess, // Some aspects were recovered but not all
    NotAttempted    // Recovery was not attempted
};

/**
 * @brief Enum for recovery strategy types
 */
enum class RecoveryStrategyType {
    Checkpoint,    // Restore from last checkpoint
    Retry,         // Retry the failed operation
    Fallback,      // Use fallback/alternative implementation
    Degraded,      // Continue with reduced functionality
    UserPrompt,    // Ask the user what to do
    Terminate      // Terminate the application gracefully
};

/**
 * @brief Information about an exception for recovery purposes
 */
struct ExceptionInfo {
    std::string message;
    std::string exceptionType;
    std::string sourceFile;
    int sourceLine;
    std::string sourceFunction;
    int errorCode;
    Exception::Severity severity;
    std::vector<std::string> context;
    std::string component;  // System component where the error occurred
    std::string operation;  // Operation that failed
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata; // Additional error details
    
    // Constructor from Exception
    explicit ExceptionInfo(const Exception& ex);
    
    // Constructor from std::exception
    explicit ExceptionInfo(const std::exception& ex, 
                        const std::string& component = "Unknown",
                        const std::string& operation = "Unknown",
                        Exception::Severity severity = Exception::Severity::Error);
};

/**
 * @brief Recovery checkpoint for game state
 * 
 * Stores essential game state that can be restored in case of a critical error
 */
class RecoveryCheckpoint {
public:
    // Constructor that captures current game state
    explicit RecoveryCheckpoint(const std::string& name);
    
    // Restore the game to this checkpoint
    RecoveryResult restore();
    
    // Get checkpoint information
    const std::string& getName() const { return m_name; }
    std::chrono::system_clock::time_point getTimestamp() const { return m_timestamp; }
    
    // Save specific state data in the checkpoint
    template<typename T>
    void saveState(const std::string& key, const T& value) {
        m_state[key] = value;
    }
    
    // Retrieve state data from the checkpoint
    template<typename T>
    std::optional<T> getState(const std::string& key) const {
        auto it = m_state.find(key);
        if (it != m_state.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }
    
private:
    std::string m_name;
    std::chrono::system_clock::time_point m_timestamp;
    std::unordered_map<std::string, std::any> m_state;
    std::function<RecoveryResult()> m_restoreFunction;
};

/**
 * @brief Abstract base class for recovery strategies
 */
class RecoveryStrategy {
public:
    virtual ~RecoveryStrategy() = default;
    
    // Apply the recovery strategy to the given exception
    virtual RecoveryResult apply(const ExceptionInfo& exInfo) = 0;
    
    // Check if this strategy can handle the given exception
    virtual bool canHandle(const ExceptionInfo& exInfo) const = 0;
    
    // Get the type of this strategy
    virtual RecoveryStrategyType getType() const = 0;
    
    // Get a human-friendly description of this strategy
    virtual std::string getDescription() const = 0;
};

/**
 * @brief Checkpoint-based recovery strategy
 * 
 * Restores the game state to a previous checkpoint when an error occurs
 */
class CheckpointRecoveryStrategy : public RecoveryStrategy {
public:
    explicit CheckpointRecoveryStrategy(std::shared_ptr<RecoveryCheckpoint> checkpoint);
    
    RecoveryResult apply(const ExceptionInfo& exInfo) override;
    bool canHandle(const ExceptionInfo& exInfo) const override;
    RecoveryStrategyType getType() const override { return RecoveryStrategyType::Checkpoint; }
    std::string getDescription() const override;
    
private:
    std::shared_ptr<RecoveryCheckpoint> m_checkpoint;
};

/**
 * @brief Retry policy for failed operations
 * 
 * Defines how and when to retry a failed operation
 */
class RetryPolicy {
public:
    // Construct with maximum retry count and delay between attempts
    RetryPolicy(int maxRetries = 3, std::chrono::milliseconds delay = std::chrono::milliseconds(100));
    
    // Set exponential backoff (each retry waits longer than the previous one)
    void setExponentialBackoff(bool enable, float factor = 2.0f);
    
    // Set a condition function that determines if a retry should be attempted
    void setRetryCondition(std::function<bool(const ExceptionInfo&, int)> condition);
    
    // Get maximum number of retries
    int getMaxRetries() const { return m_maxRetries; }
    
    // Calculate delay for a specific retry attempt
    std::chrono::milliseconds getDelayForAttempt(int attempt) const;
    
    // Check if a retry should be attempted for the given exception and attempt number
    bool shouldRetry(const ExceptionInfo& exInfo, int attemptNumber) const;
    
private:
    int m_maxRetries;
    std::chrono::milliseconds m_baseDelay;
    bool m_useExponentialBackoff = false;
    float m_backoffFactor = 2.0f;
    std::function<bool(const ExceptionInfo&, int)> m_retryCondition;
};

/**
 * @brief Retry-based recovery strategy
 * 
 * Attempts to retry the failed operation
 */
class RetryRecoveryStrategy : public RecoveryStrategy {
public:
    RetryRecoveryStrategy(
        std::function<bool()> operation,
        RetryPolicy policy = RetryPolicy(),
        const std::string& operationName = "Unknown Operation"
    );
    
    RecoveryResult apply(const ExceptionInfo& exInfo) override;
    bool canHandle(const ExceptionInfo& exInfo) const override;
    RecoveryStrategyType getType() const override { return RecoveryStrategyType::Retry; }
    std::string getDescription() const override;
    
private:
    std::function<bool()> m_operation;
    RetryPolicy m_retryPolicy;
    std::string m_operationName;
};

/**
 * @brief Fallback recovery strategy
 * 
 * Uses an alternative implementation when the primary one fails
 */
class FallbackRecoveryStrategy : public RecoveryStrategy {
public:
    FallbackRecoveryStrategy(
        std::function<bool()> fallbackOperation,
        const std::string& componentName = "Unknown Component"
    );
    
    RecoveryResult apply(const ExceptionInfo& exInfo) override;
    bool canHandle(const ExceptionInfo& exInfo) const override;
    RecoveryStrategyType getType() const override { return RecoveryStrategyType::Fallback; }
    std::string getDescription() const override;
    
private:
    std::function<bool()> m_fallbackOperation;
    std::string m_componentName;
};

/**
 * @brief Graceful degradation recovery strategy
 * 
 * Continues execution with reduced functionality
 */
class DegradedModeStrategy : public RecoveryStrategy {
public:
    DegradedModeStrategy(
        std::function<bool()> degradeFunction,
        const std::string& featureName = "Unknown Feature"
    );
    
    RecoveryResult apply(const ExceptionInfo& exInfo) override;
    bool canHandle(const ExceptionInfo& exInfo) const override;
    RecoveryStrategyType getType() const override { return RecoveryStrategyType::Degraded; }
    std::string getDescription() const override;
    
private:
    std::function<bool()> m_degradeFunction;
    std::string m_featureName;
};

/**
 * @brief User prompt recovery strategy
 * 
 * Asks the user what action to take when an error occurs
 */
class UserPromptStrategy : public RecoveryStrategy {
public:
    UserPromptStrategy(
        const std::vector<std::string>& options,
        std::function<int(const ExceptionInfo&, const std::vector<std::string>&)> promptFunction
    );
    
    RecoveryResult apply(const ExceptionInfo& exInfo) override;
    bool canHandle(const ExceptionInfo& exInfo) const override;
    RecoveryStrategyType getType() const override { return RecoveryStrategyType::UserPrompt; }
    std::string getDescription() const override;
    
    // Add an action to be performed based on user choice
    void addAction(int choice, std::function<RecoveryResult()> action);
    
private:
    std::vector<std::string> m_options;
    std::function<int(const ExceptionInfo&, const std::vector<std::string>&)> m_promptFunction;
    std::unordered_map<int, std::function<RecoveryResult()>> m_actions;
};

/**
 * @brief Exception telemetry for analytics and reporting
 */
class ExceptionTelemetry {
public:
    static ExceptionTelemetry& getInstance();
    
    // Record an exception in the telemetry system
    void recordException(const ExceptionInfo& exInfo);
    
    // Get statistics on exceptions by component
    std::unordered_map<std::string, int> getExceptionsByComponent() const;
    
    // Get statistics on exceptions by type
    std::unordered_map<std::string, int> getExceptionsByType() const;
    
    // Get most frequent exceptions
    std::vector<std::pair<std::string, int>> getTopExceptions(int count = 5) const;
    
    // Save telemetry data to a file
    bool saveTelemetryData(const std::string& filename) const;
    
private:
    ExceptionTelemetry() = default;
    
    mutable std::mutex m_mutex;
    std::vector<ExceptionInfo> m_exceptionHistory;
    std::unordered_map<std::string, int> m_exceptionsByComponent;
    std::unordered_map<std::string, int> m_exceptionsByType;
    std::unordered_map<std::string, int> m_exceptionsByMessage;
};

/**
 * @brief Central manager for exception recovery strategies
 */
class ExceptionRecoverySystem {
public:
    static ExceptionRecoverySystem& getInstance();
    
    // Register a recovery strategy
    void registerStrategy(std::shared_ptr<RecoveryStrategy> strategy);
    
    // Create and register a checkpoint
    std::shared_ptr<RecoveryCheckpoint> createCheckpoint(const std::string& name);
    
    // Handle an exception with appropriate recovery strategy
    RecoveryResult handleException(const Exception& ex);
    RecoveryResult handleException(const std::exception& ex, const std::string& component = "Unknown");
    
    // Get the latest checkpoint
    std::shared_ptr<RecoveryCheckpoint> getLatestCheckpoint() const;
    
    // Enable/disable telemetry
    void enableTelemetry(bool enable) { m_telemetryEnabled = enable; }
    bool isTelemetryEnabled() const { return m_telemetryEnabled; }
    
    // Get access to telemetry data
    ExceptionTelemetry& getTelemetry() const;
    
    // Create a default set of strategies
    void initializeDefaultStrategies();
    
    // Set user-friendly error messages for common errors
    void setErrorMessage(int errorCode, const std::string& message);
    
    // Get user-friendly error message for an error code
    std::string getErrorMessage(int errorCode) const;
    
private:
    ExceptionRecoverySystem();
    
    mutable std::mutex m_mutex;
    std::vector<std::shared_ptr<RecoveryStrategy>> m_strategies;
    std::vector<std::shared_ptr<RecoveryCheckpoint>> m_checkpoints;
    std::unordered_map<int, std::string> m_errorMessages;
    bool m_telemetryEnabled = true;
};

// Helper functions for creating common recovery strategies

/**
 * @brief Create a retry strategy for resource loading
 */
std::shared_ptr<RetryRecoveryStrategy> createResourceLoadRetryStrategy(
    const std::string& resourcePath, 
    std::function<bool(const std::string&)> loadFunction,
    int maxRetries = 3
);

/**
 * @brief Create a fallback strategy for a component
 */
std::shared_ptr<FallbackRecoveryStrategy> createComponentFallbackStrategy(
    const std::string& componentName,
    std::function<bool()> fallbackFunction
);

/**
 * @brief Create a degraded mode strategy for a feature
 */
std::shared_ptr<DegradedModeStrategy> createDegradedModeStrategy(
    const std::string& featureName,
    std::function<bool()> degradeFunction
);

/**
 * @brief Template function for safely executing an operation with recovery
 */
template<typename Func>
auto executeWithRecovery(
    Func&& func, 
    const std::string& operation, 
    const std::string& component) -> decltype(func())
{
    try {
        return func();
    }
    catch (const Exception& ex) {
        getLogger().error(std::format("Exception in {}/{}: {}", component, operation, ex.what()));
        
        ExceptionInfo info(ex);
        info.component = component;
        info.operation = operation;
        
        auto& recovery = ExceptionRecoverySystem::getInstance();
        auto result = recovery.handleException(ex);
        
        if (result != RecoveryResult::Success) {
            throw; // Rethrow if recovery failed
        }
        
        // Recovery succeeded - retry the operation
        return func();
    }
    catch (const std::exception& ex) {
        getLogger().error(std::format("Standard exception in {}/{}: {}", component, operation, ex.what()));
        
        auto& recovery = ExceptionRecoverySystem::getInstance();
        auto result = recovery.handleException(ex, component);
        
        if (result != RecoveryResult::Success) {
            throw; // Rethrow if recovery failed
        }
        
        // Recovery succeeded - retry the operation
        return func();
    }
}

} // namespace GameExceptions