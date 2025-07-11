#pragma once

#include "Plugin/IPlugin.h"
#include "Plugin/PluginFactory.h"
#include "Plugin/PluginVersion.h"

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <filesystem>

class GameSession;

/**
 * @brief Manager for loading and handling plugins
 * 
 * This class is responsible for discovering, loading, initializing,
 * updating, and unloading plugins.
 */
class PluginManager {
public:
    /**
     * @brief Get the singleton instance
     */
    static PluginManager& getInstance();
    
    /**
     * @brief Initialize the plugin manager
     * @param gameVersion Current game version
     * @param gameSession Game session pointer
     * @return True if initialization succeeded
     */
    bool initialize(const PluginVersion& gameVersion, GameSession* gameSession);
    
    /**
     * @brief Shutdown the plugin manager and all plugins
     * @return True if shutdown succeeded
     */
    bool shutdown();
    
    /**
     * @brief Update all active plugins
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Render all active plugins
     * @param window Render window
     */
    void render(sf::RenderWindow& window);
    
    /**
     * @brief Register a plugin factory
     * @param factory Factory for creating plugin instances
     * @return True if registration succeeded
     */
    bool registerPluginFactory(std::unique_ptr<IPluginFactory> factory);
    
    /**
     * @brief Register a plugin factory with template parameter
     * @tparam PluginType Type of plugin to create
     * @return True if registration succeeded
     */
    template<typename PluginType>
    bool registerPlugin() {
        return registerPluginFactory(std::make_unique<PluginFactory<PluginType>>());
    }
    
    /**
     * @brief Load a plugin by name
     * @param pluginName Name of the plugin to load
     * @return True if plugin was loaded successfully
     */
    bool loadPlugin(const std::string& pluginName);
    
    /**
     * @brief Unload a plugin by name
     * @param pluginName Name of the plugin to unload
     * @return True if plugin was unloaded successfully
     */
    bool unloadPlugin(const std::string& pluginName);
    
    /**
     * @brief Hot-reload a plugin by name
     * @param pluginName Name of the plugin to reload
     * @return True if plugin was reloaded successfully
     */
    bool reloadPlugin(const std::string& pluginName);
    
    /**
     * @brief Discover and load plugins from a directory
     * @param directory Directory to search for plugins
     * @return Number of plugins loaded
     */
    int discoverAndLoadPlugins(const std::filesystem::path& directory);
    
    /**
     * @brief Get a loaded plugin by name
     * @param pluginName Name of the plugin to get
     * @return Pointer to the plugin, or nullptr if not loaded
     */
    IPlugin* getPlugin(const std::string& pluginName);
    
    /**
     * @brief Get all loaded plugins
     * @return Vector of plugin pointers
     */
    std::vector<IPlugin*> getLoadedPlugins() const;
    
    /**
     * @brief Get all available plugin names
     * @return Vector of plugin names
     */
    std::vector<std::string> getAvailablePluginNames() const;
    
    /**
     * @brief Check if a plugin is loaded
     * @param pluginName Name of the plugin to check
     * @return True if plugin is loaded
     */
    bool isPluginLoaded(const std::string& pluginName) const;
    
    /**
     * @brief Set plugin loading error callback
     * @param callback Function to call when plugin loading fails
     */
    void setErrorCallback(std::function<void(const std::string&, const std::string&)> callback) {
        m_errorCallback = callback;
    }
    
    /**
     * @brief Get the current game version
     * @return The game version
     */
    const PluginVersion& getGameVersion() const { return m_gameVersion; }
    
private:
    PluginManager() = default;
    ~PluginManager() = default;
    
    // Prevent copying or moving
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) = delete;
    PluginManager& operator=(PluginManager&&) = delete;
    
    // Helper methods
    bool loadPluginDependencies(const std::string& pluginName);
    void reportError(const std::string& pluginName, const std::string& error);
    void sortPluginsTopologically();
    bool checkCircularDependencies();
    
    // Plugin data structures
    std::unordered_map<std::string, std::unique_ptr<IPluginFactory>> m_factories;
    std::unordered_map<std::string, std::unique_ptr<IPlugin>> m_loadedPlugins;
    std::vector<std::string> m_loadOrder; // Sorted plugin names for update order
    
    // Game data
    PluginVersion m_gameVersion;
    GameSession* m_gameSession = nullptr;
    bool m_initialized = false;
    
    // Callbacks
    std::function<void(const std::string&, const std::string&)> m_errorCallback;
};