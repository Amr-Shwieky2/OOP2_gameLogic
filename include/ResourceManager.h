// include/ResourceManager.h
#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

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
            throw std::runtime_error("Failed to load resource: " + filename);
        }

        Resource& ref = *resPtr;
        m_resources[filename] = std::move(resPtr);
        return ref;
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