#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>
#include "GameExceptions.h"

/**
 * @brief Template resource loader with caching
 */
template<typename ResourceType>
class ResourceLoader {
public:
    ResourceLoader() = default;
    ~ResourceLoader() = default;

    // Core function - same as old getTexture/getFont
    ResourceType& getResource(const std::string& filename) {
        auto it = m_resources.find(filename);
        if (it != m_resources.end()) {
            return *it->second;
        }

        auto resource = std::make_unique<ResourceType>();
        if (!loadResource(*resource, filename)) {
            THROW_RESOURCE_EXCEPTION("Failed to load resource: " + filename);
        }

        ResourceType& ref = *resource;
        m_resources[filename] = std::move(resource);
        return ref;
    }

    // === NEW FUNCTIONS (useful additions) ===

    // Check if resource exists in cache
    bool hasResource(const std::string& filename) const {
        return m_resources.find(filename) != m_resources.end();
    }

    // Number of loaded resources (useful for debugging/monitoring)
    size_t getResourceCount() const {
        return m_resources.size();
    }

    // Clear all cache (useful for memory management)
    void clear() {
        m_resources.clear();
    }

    // Remove specific resource (useful for memory management)
    void removeResource(const std::string& filename) {
        m_resources.erase(filename);
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ResourceType>> m_resources;

    // Actual loading - specialized for each type
    bool loadResource(ResourceType& resource, const std::string& filename);
};

template<>
inline bool ResourceLoader<sf::Texture>::loadResource(sf::Texture& texture, const std::string& filename) {
    return texture.loadFromFile(filename);
}

template<>
inline bool ResourceLoader<sf::Font>::loadResource(sf::Font& font, const std::string& filename) {
    return font.loadFromFile(filename);
}

template<>
inline bool ResourceLoader<sf::SoundBuffer>::loadResource(sf::SoundBuffer& sound, const std::string& filename) {
    return sound.loadFromFile(filename);
}

using TextureLoader = ResourceLoader<sf::Texture>;
using FontLoader = ResourceLoader<sf::Font>;
using SoundLoader = ResourceLoader<sf::SoundBuffer>;
