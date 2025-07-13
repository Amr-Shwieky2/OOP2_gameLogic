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

// Generic resource manager template
// All definitions are in this header for templates

template<typename Resource>
class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    // Get resource by filename, loading on demand
    Resource& getResource(const std::string& filename) {
        auto it = m_resources.find(filename);
        if (it != m_resources.end()) {
            return *it->second;
        }

        auto resPtr = std::make_unique<Resource>();
        if (!loadResource(*resPtr, filename)) {
            // Check if file exists first
            if (!std::filesystem::exists(filename)) {
                throw GameExceptions::ResourceNotFoundException(filename);
            }
            // File exists but couldn't be loaded
            throw GameExceptions::ResourceLoadException(filename, "Failed to load resource");
        }

        Resource& ref = *resPtr;
        m_resources[filename] = std::move(resPtr);
        return ref;
    }

    // Try to get a resource, returning a default if not found
    Resource& tryGetResource(const std::string& filename, Resource& defaultResource) {
        try {
            return getResource(filename);
        } 
        catch (const GameExceptions::ResourceException& ex) {
            // Log the exception but don't propagate it
            GameExceptions::getLogger().logException(ex, GameExceptions::LogLevel::Warning);
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
};

// Convenience type aliases
using TextureManager = ResourceManager<sf::Texture>;
using FontManager = ResourceManager<sf::Font>;
using SoundManager = ResourceManager<sf::SoundBuffer>;