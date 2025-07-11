#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Plugin/PluginVersion.h"

class GameSession;

/**
 * @brief Interface for all game plugins
 * 
 * This class defines the core interface that all plugins must implement.
 * It provides lifecycle methods for initialization, update, and shutdown,
 * as well as version compatibility checking.
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;

    /**
     * @brief Get the unique name of the plugin
     * @return String identifier for the plugin
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get the plugin version information
     * @return Version information
     */
    virtual PluginVersion getVersion() const = 0;
    
    /**
     * @brief Get plugin description
     * @return Description string
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * @brief Get plugin author
     * @return Author information
     */
    virtual std::string getAuthor() const = 0;
    
    /**
     * @brief Initialize the plugin
     * @param gameSession Pointer to the game session
     * @return True if initialization succeeds, false otherwise
     */
    virtual bool initialize(GameSession* gameSession) = 0;
    
    /**
     * @brief Update plugin state
     * @param deltaTime Time elapsed since last frame in seconds
     */
    virtual void update(float deltaTime) = 0;
    
    /**
     * @brief Render any plugin-specific visuals
     * @param window The render window
     */
    virtual void render(sf::RenderWindow& window) = 0;
    
    /**
     * @brief Shutdown the plugin
     * @return True if shutdown succeeds, false otherwise
     */
    virtual bool shutdown() = 0;
    
    /**
     * @brief Check if plugin is compatible with current game version
     * @param gameVersion The current game version
     * @return True if compatible, false otherwise
     */
    virtual bool isCompatible(const PluginVersion& gameVersion) const = 0;
    
    /**
     * @brief Get dependencies for this plugin
     * @return Vector of plugin names this plugin depends on
     */
    virtual std::vector<std::string> getDependencies() const = 0;
    
    /**
     * @brief Get plugin's primary category
     * @return Category string (e.g., "Gameplay", "Graphics", "Audio", etc.)
     */
    virtual std::string getCategory() const = 0;
    
    /**
     * @brief Check if plugin can be hot-reloaded while the game is running
     * @return True if plugin supports hot-reload
     */
    virtual bool supportsHotReload() const = 0;
};