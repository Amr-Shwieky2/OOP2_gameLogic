#include "Exceptions/ExceptionRecoverySystem.h"
#include "Exceptions/Logger.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>

namespace GameExceptions {

//-------------------------------------------------------------------------
// ExceptionInfo Implementation
//-------------------------------------------------------------------------

ExceptionInfo::ExceptionInfo(const Exception& ex) :
    message(ex.getMessage()),
    exceptionType(typeid(ex).name()),
    sourceFile(ex.getFile()),
    sourceLine(ex.getLine()),
    sourceFunction(ex.getFunction()),
    errorCode(ex.getErrorCode()),
    severity(ex.getSeverity()),
    timestamp(std::chrono::system_clock::now()) {
    
    // Copy context
    context = ex.getContextVector();
}

ExceptionInfo::ExceptionInfo(const std::exception& ex, 
                          const std::string& component,
                          const std::string& operation,
                          Exception::Severity severity) :
    message(ex.what()),
    exceptionType(typeid(ex).name()),
    sourceLine(0),
    errorCode(0),
    severity(severity),
    component(component),
    operation(operation),
    timestamp(std::chrono::system_clock::now()) {
    
    // Standard exceptions don't have the context information
}

//-------------------------------------------------------------------------
// RecoveryCheckpoint Implementation
//-------------------------------------------------------------------------

RecoveryCheckpoint::RecoveryCheckpoint(const std::string& name) :
    m_name(name),
    m_timestamp(std::chrono::system_clock::now()) {
    
    getLogger().info(std::format("Created recovery checkpoint: {}", name));
}

RecoveryResult RecoveryCheckpoint::restore() {
    if (m_restoreFunction) {
        getLogger().info(std::format("Restoring checkpoint: {}", m_name));
        return m_restoreFunction();
    }
    
    getLogger().warning(std::format("No restore function set for checkpoint: {}", m_name));
    return RecoveryResult::NotAttempted;
}

//-------------------------------------------------------------------------
// RetryPolicy Implementation
//-------------------------------------------------------------------------

RetryPolicy::RetryPolicy(int maxRetries, std::chrono::milliseconds delay) :
    m_maxRetries(maxRetries),
    m_baseDelay(delay),
    m_retryCondition([](const ExceptionInfo&, int attempt) { return true; }) {
}

void RetryPolicy::setExponentialBackoff(bool enable, float factor) {
    m_useExponentialBackoff = enable;
    m_backoffFactor = factor;
}

void RetryPolicy::setRetryCondition(std::function<bool(const ExceptionInfo&, int)> condition) {
    m_retryCondition = condition;
}

std::chrono::milliseconds RetryPolicy::getDelayForAttempt(int attempt) const {
    if (!m_useExponentialBackoff) {
        return m_baseDelay;
    }
    
    // Calculate exponential backoff: baseDelay * (factor^attempt)
    float multiplier = std::pow(m_backoffFactor, attempt - 1);
    auto delay = std::chrono::milliseconds(
        static_cast<long long>(m_baseDelay.count() * multiplier)
    );
    
    return delay;
}

bool RetryPolicy::shouldRetry(const ExceptionInfo& exInfo, int attemptNumber) const {
    // Check if we've exceeded the maximum retries
    if (attemptNumber > m_maxRetries) {
        return false;
    }
    
    // Use the retry condition to determine if we should retry
    return m_retryCondition(exInfo, attemptNumber);
}

//-------------------------------------------------------------------------
// CheckpointRecoveryStrategy Implementation
//-------------------------------------------------------------------------

CheckpointRecoveryStrategy::CheckpointRecoveryStrategy(std::shared_ptr<RecoveryCheckpoint> checkpoint) :
    m_checkpoint(checkpoint) {
}

RecoveryResult CheckpointRecoveryStrategy::apply(const ExceptionInfo& exInfo) {
    if (!m_checkpoint) {
        getLogger().error("Cannot apply checkpoint strategy: no checkpoint set");
        return RecoveryResult::NotAttempted;
    }
    
    getLogger().info(std::format("Applying checkpoint recovery strategy: {}", m_checkpoint->getName()));
    
    return m_checkpoint->restore();
}

bool CheckpointRecoveryStrategy::canHandle(const ExceptionInfo& exInfo) const {
    // Checkpoint strategy can typically handle severe errors
    return exInfo.severity >= Exception::Severity::Critical;
}

std::string CheckpointRecoveryStrategy::getDescription() const {
    return std::format("Restore checkpoint: {}", 
        m_checkpoint ? m_checkpoint->getName() : "null");
}

//-------------------------------------------------------------------------
// RetryRecoveryStrategy Implementation
//-------------------------------------------------------------------------

RetryRecoveryStrategy::RetryRecoveryStrategy(
    std::function<bool()> operation,
    RetryPolicy policy,
    const std::string& operationName
) :
    m_operation(operation),
    m_retryPolicy(policy),
    m_operationName(operationName) {
}

RecoveryResult RetryRecoveryStrategy::apply(const ExceptionInfo& exInfo) {
    int attempt = 1;
    bool success = false;
    
    getLogger().info(std::format("Applying retry recovery strategy for: {}", m_operationName));
    
    while (!success && m_retryPolicy.shouldRetry(exInfo, attempt)) {
        getLogger().info(std::format("Retry attempt {} for {}", attempt, m_operationName));
        
        // Wait before retry (except for first attempt)
        if (attempt > 1) {
            auto delay = m_retryPolicy.getDelayForAttempt(attempt);
            std::this_thread::sleep_for(delay);
        }
        
        try {
            success = m_operation();
            if (success) {
                getLogger().info(std::format("Retry succeeded on attempt {}", attempt));
                return RecoveryResult::Success;
            }
        } 
        catch (const std::exception& ex) {
            getLogger().warning(std::format("Retry attempt {} failed: {}", attempt, ex.what()));
        }
        
        attempt++;
    }
    
    getLogger().error(std::format("All retry attempts failed for {}", m_operationName));
    return RecoveryResult::Failure;
}

bool RetryRecoveryStrategy::canHandle(const ExceptionInfo& exInfo) const {
    // Retry is typically suitable for transient errors like I/O, network, etc.
    // and for less severe errors
    return exInfo.severity <= Exception::Severity::Error;
}

std::string RetryRecoveryStrategy::getDescription() const {
    return std::format("Retry operation: {} (max {} attempts)", 
        m_operationName, m_retryPolicy.getMaxRetries());
}

//-------------------------------------------------------------------------
// FallbackRecoveryStrategy Implementation
//-------------------------------------------------------------------------

FallbackRecoveryStrategy::FallbackRecoveryStrategy(
    std::function<bool()> fallbackOperation,
    const std::string& componentName
) :
    m_fallbackOperation(fallbackOperation),
    m_componentName(componentName) {
}

RecoveryResult FallbackRecoveryStrategy::apply(const ExceptionInfo& exInfo) {
    getLogger().info(std::format("Applying fallback strategy for component: {}", m_componentName));
    
    try {
        bool result = m_fallbackOperation();
        if (result) {
            getLogger().info(std::format("Fallback succeeded for component: {}", m_componentName));
            return RecoveryResult::Success;
        } else {
            getLogger().warning(std::format("Fallback returned false for component: {}", m_componentName));
            return RecoveryResult::Failure;
        }
    } 
    catch (const std::exception& ex) {
        getLogger().error(std::format("Fallback failed for component {}: {}", m_componentName, ex.what()));
        return RecoveryResult::Failure;
    }
}

bool FallbackRecoveryStrategy::canHandle(const ExceptionInfo& exInfo) const {
    // Fallback is suitable for component-specific errors where an alternative exists
    return exInfo.component == m_componentName;
}

std::string FallbackRecoveryStrategy::getDescription() const {
    return std::format("Use fallback for component: {}", m_componentName);
}

//-------------------------------------------------------------------------
// DegradedModeStrategy Implementation
//-------------------------------------------------------------------------

DegradedModeStrategy::DegradedModeStrategy(
    std::function<bool()> degradeFunction,
    const std::string& featureName
) :
    m_degradeFunction(degradeFunction),
    m_featureName(featureName) {
}

RecoveryResult DegradedModeStrategy::apply(const ExceptionInfo& exInfo) {
    getLogger().info(std::format("Entering degraded mode for feature: {}", m_featureName));
    
    try {
        bool result = m_degradeFunction();
        if (result) {
            getLogger().info(std::format("Successfully entered degraded mode for: {}", m_featureName));
            return RecoveryResult::PartialSuccess; // It's a partial success because we're running with reduced functionality
        } else {
            getLogger().warning(std::format("Failed to enter degraded mode for: {}", m_featureName));
            return RecoveryResult::Failure;
        }
    } 
    catch (const std::exception& ex) {
        getLogger().error(std::format("Error entering degraded mode for {}: {}", m_featureName, ex.what()));
        return RecoveryResult::Failure;
    }
}

bool DegradedModeStrategy::canHandle(const ExceptionInfo& exInfo) const {
    // Degraded mode is typically for non-critical features
    return exInfo.severity <= Exception::Severity::Error;
}

std::string DegradedModeStrategy::getDescription() const {
    return std::format("Enter degraded mode for feature: {}", m_featureName);
}

//-------------------------------------------------------------------------
// UserPromptStrategy Implementation
//-------------------------------------------------------------------------

UserPromptStrategy::UserPromptStrategy(
    const std::vector<std::string>& options,
    std::function<int(const ExceptionInfo&, const std::vector<std::string>&)> promptFunction
) :
    m_options(options),
    m_promptFunction(promptFunction) {
}

RecoveryResult UserPromptStrategy::apply(const ExceptionInfo& exInfo) {
    getLogger().info("Applying user prompt recovery strategy");
    
    try {
        int choice = m_promptFunction(exInfo, m_options);
        
        auto it = m_actions.find(choice);
        if (it != m_actions.end()) {
            getLogger().info(std::format("Executing user choice {}", choice));
            return it->second();
        } 
        else {
            getLogger().warning(std::format("No action defined for user choice {}", choice));
            return RecoveryResult::NotAttempted;
        }
    }
    catch (const std::exception& ex) {
        getLogger().error(std::format("Error in user prompt strategy: {}", ex.what()));
        return RecoveryResult::Failure;
    }
}

bool UserPromptStrategy::canHandle(const ExceptionInfo& exInfo) const {
    // User prompts can handle most errors where user input would be helpful
    return exInfo.severity <= Exception::Severity::Critical;
}

void UserPromptStrategy::addAction(int choice, std::function<RecoveryResult()> action) {
    m_actions[choice] = action;
}

std::string UserPromptStrategy::getDescription() const {
    std::stringstream ss;
    ss << "Prompt user with options: ";
    for (size_t i = 0; i < m_options.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << i << ": " << m_options[i];
    }
    return ss.str();
}

//-------------------------------------------------------------------------
// ExceptionTelemetry Implementation
//-------------------------------------------------------------------------

ExceptionTelemetry& ExceptionTelemetry::getInstance() {
    static ExceptionTelemetry instance;
    return instance;
}

void ExceptionTelemetry::recordException(const ExceptionInfo& exInfo) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Store the exception info
    m_exceptionHistory.push_back(exInfo);
    
    // Update statistics
    m_exceptionsByComponent[exInfo.component]++;
    m_exceptionsByType[exInfo.exceptionType]++;
    m_exceptionsByMessage[exInfo.message]++;
}

std::unordered_map<std::string, int> ExceptionTelemetry::getExceptionsByComponent() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_exceptionsByComponent;
}

std::unordered_map<std::string, int> ExceptionTelemetry::getExceptionsByType() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_exceptionsByType;
}

std::vector<std::pair<std::string, int>> ExceptionTelemetry::getTopExceptions(int count) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::pair<std::string, int>> result(m_exceptionsByMessage.begin(), m_exceptionsByMessage.end());
    
    // Sort by frequency (highest first)
    std::sort(result.begin(), result.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Limit to requested count
    if (result.size() > static_cast<size_t>(count)) {
        result.resize(count);
    }
    
    return result;
}

bool ExceptionTelemetry::saveTelemetryData(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        std::ofstream file(filename);
        if (!file) {
            return false;
        }
        
        // Write header
        file << "Exception Telemetry Report\n";
        file << "=========================\n\n";
        
        // Write top exceptions
        file << "Top Exceptions:\n";
        auto topExceptions = getTopExceptions(10);
        for (const auto& [message, count] : topExceptions) {
            file << "  - " << message << ": " << count << " occurrences\n";
        }
        file << "\n";
        
        // Write exceptions by component
        file << "Exceptions by Component:\n";
        for (const auto& [component, count] : m_exceptionsByComponent) {
            file << "  - " << component << ": " << count << " occurrences\n";
        }
        file << "\n";
        
        // Write exceptions by type
        file << "Exceptions by Type:\n";
        for (const auto& [type, count] : m_exceptionsByType) {
            file << "  - " << type << ": " << count << " occurrences\n";
        }
        file << "\n";
        
        // Write full exception history
        file << "Full Exception History:\n";
        for (const auto& exInfo : m_exceptionHistory) {
            file << "  - " << exInfo.timestamp << ": " << exInfo.message << "\n";
            file << "    Component: " << exInfo.component << "\n";
            file << "    Operation: " << exInfo.operation << "\n";
            file << "    Severity: " << static_cast<int>(exInfo.severity) << "\n";
            file << "    Location: " << exInfo.sourceFile << ":" << exInfo.sourceLine << "\n";
            file << "\n";
        }
        
        return true;
    } 
    catch (const std::exception& ex) {
        getLogger().error(std::format("Error saving telemetry data: {}", ex.what()));
        return false;
    }
}

//-------------------------------------------------------------------------
// ExceptionRecoverySystem Implementation
//-------------------------------------------------------------------------

ExceptionRecoverySystem::ExceptionRecoverySystem() {
    // Set some default error messages
    m_errorMessages[101] = "Resource not found. Please check if the game files are properly installed.";
    m_errorMessages[102] = "Failed to load resource. The file might be corrupted or in an unsupported format.";
    m_errorMessages[103] = "Invalid resource format. Please reinstall the game or verify file integrity.";
}

ExceptionRecoverySystem& ExceptionRecoverySystem::getInstance() {
    static ExceptionRecoverySystem instance;
    return instance;
}

void ExceptionRecoverySystem::registerStrategy(std::shared_ptr<RecoveryStrategy> strategy) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_strategies.push_back(strategy);
    
    getLogger().debug(std::format("Registered recovery strategy: {}", strategy->getDescription()));
}

std::shared_ptr<RecoveryCheckpoint> ExceptionRecoverySystem::createCheckpoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto checkpoint = std::make_shared<RecoveryCheckpoint>(name);
    m_checkpoints.push_back(checkpoint);
    
    getLogger().debug(std::format("Created checkpoint: {}", name));
    
    return checkpoint;
}

RecoveryResult ExceptionRecoverySystem::handleException(const Exception& ex) {
    ExceptionInfo exInfo(ex);
    
    getLogger().info(std::format("Handling exception: {} (Severity: {})", 
                               ex.getMessage(), static_cast<int>(ex.getSeverity())));
    
    // Record exception in telemetry if enabled
    if (m_telemetryEnabled) {
        getTelemetry().recordException(exInfo);
    }
    
    // Find applicable strategies
    std::vector<std::shared_ptr<RecoveryStrategy>> applicableStrategies;
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& strategy : m_strategies) {
            if (strategy->canHandle(exInfo)) {
                applicableStrategies.push_back(strategy);
            }
        }
    }
    
    // If no strategies are applicable, return NotAttempted
    if (applicableStrategies.empty()) {
        getLogger().warning(std::format("No applicable recovery strategies for exception: {}", ex.getMessage()));
        return RecoveryResult::NotAttempted;
    }
    
    // Try each applicable strategy until one succeeds
    for (const auto& strategy : applicableStrategies) {
        getLogger().info(std::format("Trying recovery strategy: {}", strategy->getDescription()));
        
        RecoveryResult result = strategy->apply(exInfo);
        if (result == RecoveryResult::Success || result == RecoveryResult::PartialSuccess) {
            getLogger().info(std::format("Recovery strategy succeeded: {}", strategy->getDescription()));
            return result;
        }
        
        getLogger().info(std::format("Recovery strategy failed: {}", strategy->getDescription()));
    }
    
    // If we get here, all strategies failed
    getLogger().error(std::format("All recovery strategies failed for exception: {}", ex.getMessage()));
    return RecoveryResult::Failure;
}

RecoveryResult ExceptionRecoverySystem::handleException(
    const std::exception& ex, 
    const std::string& component) 
{
    // Convert standard exception to ExceptionInfo
    ExceptionInfo exInfo(ex, component, "Unknown", Exception::Severity::Error);
    
    getLogger().info(std::format("Handling standard exception in component {}: {}", 
                               component, ex.what()));
    
    // Record exception in telemetry if enabled
    if (m_telemetryEnabled) {
        getTelemetry().recordException(exInfo);
    }
    
    // Find applicable strategies
    std::vector<std::shared_ptr<RecoveryStrategy>> applicableStrategies;
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& strategy : m_strategies) {
            if (strategy->canHandle(exInfo)) {
                applicableStrategies.push_back(strategy);
            }
        }
    }
    
    // If no strategies are applicable, return NotAttempted
    if (applicableStrategies.empty()) {
        getLogger().warning(std::format("No applicable recovery strategies for exception: {}", ex.what()));
        return RecoveryResult::NotAttempted;
    }
    
    // Try each applicable strategy until one succeeds
    for (const auto& strategy : applicableStrategies) {
        getLogger().info(std::format("Trying recovery strategy: {}", strategy->getDescription()));
        
        RecoveryResult result = strategy->apply(exInfo);
        if (result == RecoveryResult::Success || result == RecoveryResult::PartialSuccess) {
            getLogger().info(std::format("Recovery strategy succeeded: {}", strategy->getDescription()));
            return result;
        }
        
        getLogger().info(std::format("Recovery strategy failed: {}", strategy->getDescription()));
    }
    
    // If we get here, all strategies failed
    getLogger().error(std::format("All recovery strategies failed for exception: {}", ex.what()));
    return RecoveryResult::Failure;
}

std::shared_ptr<RecoveryCheckpoint> ExceptionRecoverySystem::getLatestCheckpoint() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_checkpoints.empty()) {
        return nullptr;
    }
    
    return m_checkpoints.back();
}

ExceptionTelemetry& ExceptionRecoverySystem::getTelemetry() const {
    return ExceptionTelemetry::getInstance();
}

void ExceptionRecoverySystem::initializeDefaultStrategies() {
    // Default strategy for resource loading errors
    auto resourceRetryStrategy = std::make_shared<RetryRecoveryStrategy>(
        []() { return false; }, // Placeholder, will be replaced in actual implementation
        RetryPolicy(3, std::chrono::milliseconds(500)),
        "Resource Loading"
    );
    
    registerStrategy(resourceRetryStrategy);
    
    // Default fallback strategy for rendering
    auto renderingFallback = std::make_shared<FallbackRecoveryStrategy>(
        []() { 
            // Switch to minimal rendering mode
            getLogger().info("Switching to minimal rendering mode");
            return true;
        },
        "Rendering"
    );
    
    registerStrategy(renderingFallback);
    
    // Default degraded mode strategy for non-critical features
    auto audioFallback = std::make_shared<DegradedModeStrategy>(
        []() {
            // Disable audio
            getLogger().info("Disabling audio due to errors");
            return true;
        },
        "Audio"
    );
    
    registerStrategy(audioFallback);
    
    getLogger().info("Default recovery strategies initialized");
}

void ExceptionRecoverySystem::setErrorMessage(int errorCode, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_errorMessages[errorCode] = message;
}

std::string ExceptionRecoverySystem::getErrorMessage(int errorCode) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_errorMessages.find(errorCode);
    if (it != m_errorMessages.end()) {
        return it->second;
    }
    
    return "An error occurred. Please try again.";
}

//-------------------------------------------------------------------------
// Helper Functions
//-------------------------------------------------------------------------

std::shared_ptr<RetryRecoveryStrategy> createResourceLoadRetryStrategy(
    const std::string& resourcePath, 
    std::function<bool(const std::string&)> loadFunction,
    int maxRetries)
{
    // Create a retry function that attempts to load the resource
    auto operation = [=]() -> bool {
        try {
            return loadFunction(resourcePath);
        }
        catch (const std::exception& ex) {
            getLogger().warning(std::format("Resource load attempt failed: {}", ex.what()));
            return false;
        }
    };
    
    // Create retry policy
    RetryPolicy policy(maxRetries, std::chrono::milliseconds(500));
    policy.setExponentialBackoff(true, 2.0f);
    
    // Create retry strategy
    return std::make_shared<RetryRecoveryStrategy>(
        operation,
        policy,
        "Load Resource: " + resourcePath
    );
}

std::shared_ptr<FallbackRecoveryStrategy> createComponentFallbackStrategy(
    const std::string& componentName,
    std::function<bool()> fallbackFunction)
{
    return std::make_shared<FallbackRecoveryStrategy>(
        fallbackFunction,
        componentName
    );
}

std::shared_ptr<DegradedModeStrategy> createDegradedModeStrategy(
    const std::string& featureName,
    std::function<bool()> degradeFunction)
{
    return std::make_shared<DegradedModeStrategy>(
        degradeFunction,
        featureName
    );
}

} // namespace GameExceptions