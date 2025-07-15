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


template<typename Resource>
class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    Resource& getResource(const std::string& filename) {
        auto it = m_resources.find(filename);
        if (it != m_resources.end()) {
            return *it->second;
        }

        auto resPtr = std::make_unique<Resource>();
        if (!loadResource(*resPtr, filename)) {
            if (!std::filesystem::exists(filename)) {
                GameExceptions::getLogger().log(GameExceptions::LogLevel::Error, "[RESOURCE] File does not exist: " + filename);
                throw GameExceptions::ResourceNotFoundException(filename);
            }
            GameExceptions::getLogger().log(GameExceptions::LogLevel::Error, "[RESOURCE] Failed to load: " + filename);
            throw GameExceptions::ResourceLoadException(filename, "Failed to load resource");
        }

        Resource& ref = *resPtr;
        m_resources[filename] = std::move(resPtr);
        return ref;
    }


    // Try get with fallback resource
    Resource& tryGetResource(const std::string& filename, Resource& defaultResource) {
        try {
            return getResource(filename);
        }
        catch (const GameExceptions::ResourceException& ex) {
            GameExceptions::getLogger().logException(ex, GameExceptions::LogLevel::Warning);
            return defaultResource;
        }
    }

    // Check if resource is already loaded
    bool isLoaded(const std::string& filename) const {
        return m_resources.find(filename) != m_resources.end();
    }

    // Preload resource
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

    // Release specific resource
    void releaseResource(const std::string& filename) {
        m_resources.erase(filename);
    }

    // Release all resources
    void releaseAllResources() {
        m_resources.clear();
    }

    // Number of loaded resources
    size_t getResourceCount() const {
        return m_resources.size();
    }

private:
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

using TextureManager = ResourceManager<sf::Texture>;
using FontManager = ResourceManager<sf::Font>;
using SoundManager = ResourceManager<sf::SoundBuffer>;