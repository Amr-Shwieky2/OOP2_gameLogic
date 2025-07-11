// include/ResourceManager.h
#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Exceptions/GameExceptions.h"
#include "Exceptions/Logger.h"
#include "Exceptions/ResourceRetry.h"
#include "Exceptions/ExceptionRecoverySystem.h"
#include <format>

// Generic resource manager template
// All definitions are in this header for templates

template<typename Resource>
class ResourceManager {
public:
    ResourceManager() : m_maxRetries(3) {}
    ~ResourceManager() = default;

    // Set the maximum number of retries for resource loading
    void setMaxRetries(int maxRetries) {
        m_maxRetries = maxRetries;
    }

    // Get resource by filename, loading on demand with recovery
    Resource& getResource(const std::string& filename) {
        auto it = m_resources.find(filename);
        if (it != m_resources.end()) {
            return *it->second;
        }

        try {
            // Use our retry mechanism for loading resources
            auto resource = GameExceptions::loadResourceWithRetry<Resource>(
                filename,
                [this](const std::string& path) {
                    auto resPtr = std::make_unique<Resource>();
                    if (!loadResource(*resPtr, path)) {
                        // Check if file exists first
                        if (!std::filesystem::exists(path)) {
                            throw GameExceptions::ResourceNotFoundException(path);
                        }
                        // File exists but couldn't be loaded
                        throw GameExceptions::ResourceLoadException(path, "Failed to load resource");
                    }
                    return *resPtr;
                },
                m_maxRetries
            );
            
            // Store the loaded resource
            auto resPtr = std::make_unique<Resource>(std::move(resource));
            Resource& ref = *resPtr;
            m_resources[filename] = std::move(resPtr);
            return ref;
        }
        catch (const GameExceptions::ResourceNotFoundException& ex) {
            GameExceptions::getLogger().error(std::format("Resource not found after {} retries: {}", 
                                                        m_maxRetries, filename));
                                                        
            // Try to use fallback resource if available
            auto fallback = findFallbackResource(filename);
            if (!fallback.empty()) {
                GameExceptions::getLogger().info(std::format("Using fallback resource: {} for {}", 
                                                          fallback, filename));
                return getResource(fallback);
            }
            
            // Use exception recovery system as last resort
            auto& recoverySystem = GameExceptions::ExceptionRecoverySystem::getInstance();
            auto result = recoverySystem.handleException(ex);
            
            if (result == GameExceptions::RecoveryResult::Success) {
                // Recovery succeeded, try again
                return getResource(filename);
            }
            
            // If all recovery attempts failed, throw
            throw;
        }
        catch (const GameExceptions::ResourceLoadException& ex) {
            GameExceptions::getLogger().error(std::format("Failed to load resource after {} retries: {}", 
                                                        m_maxRetries, filename));
                                                        
            // Try to use fallback resource if available
            auto fallback = findFallbackResource(filename);
            if (!fallback.empty()) {
                GameExceptions::getLogger().info(std::format("Using fallback resource: {} for {}", 
                                                          fallback, filename));
                return getResource(fallback);
            }
            
            // Use exception recovery system as last resort
            auto& recoverySystem = GameExceptions::ExceptionRecoverySystem::getInstance();
            auto result = recoverySystem.handleException(ex);
            
            if (result == GameExceptions::RecoveryResult::Success) {
                // Recovery succeeded, try again
                return getResource(filename);
            }
            
            // If all recovery attempts failed, throw
            throw;
        }
    }

    // Try to get a resource, returning a default if not found
    Resource& tryGetResource(const std::string& filename, Resource& defaultResource) {
        try {
            return getResource(filename);
        } 
        catch (const GameExceptions::ResourceException& ex) {
            // Log the exception but don't propagate it
            GameExceptions::getLogger().logException(ex, GameExceptions::LogLevel::Warning);
            
            // Record in telemetry for analysis
            if (GameExceptions::ExceptionRecoverySystem::getInstance().isTelemetryEnabled()) {
                GameExceptions::ExceptionInfo info(ex);
                info.component = "ResourceManager";
                info.operation = "tryGetResource";
                GameExceptions::ExceptionRecoverySystem::getInstance().getTelemetry().recordException(info);
            }
            
            return defaultResource;
        }
    }

    // Check if resource is loaded
    bool isLoaded(const std::string& filename) const {
        return m_resources.find(filename) != m_resources.end();
    }

    // Pre-load a resource
    bool preload(const std::string& filename) {
        try {
            getResource(filename);
            return true;
        } 
        catch (const std::exception& ex) {
            GameExceptions::getLogger().logException(ex);
            return false;
        }
    }

    // Register a fallback resource for when a primary resource can't be found
    void registerFallback(const std::string& primaryResource, const std::string& fallbackResource) {
        m_fallbacks[primaryResource] = fallbackResource;
        GameExceptions::getLogger().info(std::format("Registered fallback for {}: {}", 
                                                  primaryResource, fallbackResource));
    }
    
    // Find a registered fallback resource
    std::string findFallbackResource(const std::string& resource) const {
        auto it = m_fallbacks.find(resource);
        if (it != m_fallbacks.end()) {
            return it->second;
        }
        
        // No exact match, try finding by extension
        try {
            std::filesystem::path path(resource);
            std::string extension = path.extension().string();
            
            // Check for fallback for this file type
            std::string fallbackKey = "*" + extension;
            auto typeIt = m_fallbacks.find(fallbackKey);
            if (typeIt != m_fallbacks.end()) {
                return typeIt->second;
            }
        }
        catch (const std::exception& ex) {
            GameExceptions::getLogger().warning(std::format("Error finding fallback by extension: {}", ex.what()));
        }
        
        return ""; // No fallback found
    }

    // Release a specific resource
    void releaseResource(const std::string& filename) {
        auto it = m_resources.find(filename);
        if (it != m_resources.end()) {
            m_resources.erase(it);
        }
    }

    // Release all resources
    void releaseAllResources() {
        m_resources.clear();
    }

private:
    // Overload resolution for different SFML types
    bool loadResource(sf::Texture& resource, const std::string& filename) {
        return resource.loadFromFile(filename);
    }
    bool loadResource(sf::Font& resource, const std::string& filename) {
        return resource.loadFromFile(filename);
    }
    bool loadResource(sf::SoundBuffer& resource, const std::string& filename) {
        return resource.loadFromFile(filename);
    }

    std::unordered_map<std::string, std::unique_ptr<Resource>> m_resources;
    std::unordered_map<std::string, std::string> m_fallbacks;
    int m_maxRetries;
};

// Convenience type aliases
using TextureManager = ResourceManager<sf::Texture>;
using FontManager = ResourceManager<sf::Font>;
using SoundManager = ResourceManager<sf::SoundBuffer>;