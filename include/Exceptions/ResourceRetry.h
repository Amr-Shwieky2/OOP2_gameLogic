#pragma once

#include "Exceptions/ExceptionRecoverySystem.h"
#include "Exceptions/GameExceptions.h"
#include "Exceptions/SafeResource.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <chrono>

namespace GameExceptions {

/**
 * @brief Resource loading retry manager
 * 
 * Provides retry capabilities for loading resources with configurable policies
 */
class ResourceRetryManager {
public:
    static ResourceRetryManager& getInstance();
    
    /**
     * @brief Load a resource with retry capability
     * 
     * @tparam T Resource type
     * @param resourcePath Resource path/name
     * @param loadFunc Function to load the resource
     * @param maxRetries Maximum number of retries (default: 3)
     * @return T The loaded resource
     * @throws ResourceNotFoundException if resource not found after retries
     * @throws ResourceLoadException if resource couldn't be loaded after retries
     */
    template<typename T>
    T loadWithRetry(
        const std::string& resourcePath,
        std::function<T(const std::string&)> loadFunc,
        int maxRetries = 3)
    {
        RetryPolicy policy(maxRetries, std::chrono::milliseconds(500));
        policy.setExponentialBackoff(true, 2.0f);
        
        for (int attempt = 1; attempt <= maxRetries + 1; ++attempt) {
            try {
                getLogger().info(std::format("Loading resource {} (attempt {}/{})", 
                                           resourcePath, attempt, maxRetries + 1));
                
                T resource = loadFunc(resourcePath);
                
                // If we get here, loading was successful
                if (attempt > 1) {
                    getLogger().info(std::format("Resource {} loaded successfully after {} attempts", 
                                               resourcePath, attempt));
                }
                
                return resource;
            }
            catch (const ResourceNotFoundException& e) {
                if (attempt <= maxRetries) {
                    getLogger().warning(std::format("Resource not found: {}. Retrying ({}/{})",
                                                  resourcePath, attempt, maxRetries));
                                                  
                    auto delay = policy.getDelayForAttempt(attempt);
                    std::this_thread::sleep_for(delay);
                }
                else {
                    // All retries exhausted
                    getLogger().error(std::format("Resource not found after {} attempts: {}", 
                                                maxRetries + 1, resourcePath));
                    
                    // Try alternative paths if available
                    auto alternativePath = findAlternativePath(resourcePath);
                    if (!alternativePath.empty() && alternativePath != resourcePath) {
                        getLogger().info(std::format("Trying alternative path: {}", alternativePath));
                        try {
                            return loadFunc(alternativePath);
                        }
                        catch (const std::exception& altEx) {
                            getLogger().error(std::format("Alternative path also failed: {}", altEx.what()));
                        }
                    }
                    
                    throw; // Rethrow after all retries and alternatives failed
                }
            }
            catch (const ResourceLoadException& e) {
                if (attempt <= maxRetries) {
                    getLogger().warning(std::format("Resource load failed: {}. Retrying ({}/{})",
                                                  e.what(), attempt, maxRetries));
                                                  
                    auto delay = policy.getDelayForAttempt(attempt);
                    std::this_thread::sleep_for(delay);
                }
                else {
                    // All retries exhausted
                    getLogger().error(std::format("Resource load failed after {} attempts: {}", 
                                                maxRetries + 1, e.what()));
                    
                    // Try alternative format if available
                    auto alternativeFormat = findAlternativeFormat(resourcePath);
                    if (!alternativeFormat.empty() && alternativeFormat != resourcePath) {
                        getLogger().info(std::format("Trying alternative format: {}", alternativeFormat));
                        try {
                            return loadFunc(alternativeFormat);
                        }
                        catch (const std::exception& altEx) {
                            getLogger().error(std::format("Alternative format also failed: {}", altEx.what()));
                        }
                    }
                    
                    throw; // Rethrow after all retries and alternatives failed
                }
            }
            catch (const std::exception& e) {
                if (attempt <= maxRetries) {
                    getLogger().warning(std::format("Error loading resource: {}. Retrying ({}/{})",
                                                  e.what(), attempt, maxRetries));
                                                  
                    auto delay = policy.getDelayForAttempt(attempt);
                    std::this_thread::sleep_for(delay);
                }
                else {
                    // All retries exhausted
                    getLogger().error(std::format("Resource load failed after {} attempts: {}", 
                                                maxRetries + 1, e.what()));
                    throw; // Rethrow after all retries failed
                }
            }
        }
        
        // This should never be reached due to the loop and exception handling
        throw ResourceLoadException(resourcePath, "Unknown error in loadWithRetry");
    }
    
    /**
     * @brief Register an alternative path for a resource
     * 
     * If the primary path fails, the system will try this alternative
     */
    void registerAlternativePath(const std::string& primaryPath, const std::string& alternativePath);
    
    /**
     * @brief Register an alternative format for a resource
     * 
     * If loading the primary format fails, the system will try this alternative
     */
    void registerAlternativeFormat(const std::string& primaryFormat, const std::string& alternativeFormat);
    
    /**
     * @brief Find an alternative path for a resource
     * 
     * @param resourcePath The original resource path
     * @return Alternative path, or empty string if none available
     */
    std::string findAlternativePath(const std::string& resourcePath) const;
    
    /**
     * @brief Find an alternative format for a resource
     * 
     * @param resourcePath The original resource path
     * @return Alternative format path, or empty string if none available
     */
    std::string findAlternativeFormat(const std::string& resourcePath) const;
    
private:
    ResourceRetryManager() = default;
    
    std::unordered_map<std::string, std::string> m_alternativePaths;
    std::unordered_map<std::string, std::string> m_alternativeFormats;
    
    // Helper to replace file extension
    std::string replaceExtension(const std::string& path, const std::string& newExtension) const;
};

/**
 * @brief Helper function for loading resources with retry
 * 
 * @tparam T Resource type
 * @param resourcePath Resource path/name
 * @param loadFunc Function to load the resource
 * @param maxRetries Maximum number of retries
 * @return T The loaded resource
 */
template<typename T>
T loadResourceWithRetry(
    const std::string& resourcePath,
    std::function<T(const std::string&)> loadFunc,
    int maxRetries = 3)
{
    return ResourceRetryManager::getInstance().loadWithRetry(
        resourcePath, loadFunc, maxRetries);
}

} // namespace GameExceptions