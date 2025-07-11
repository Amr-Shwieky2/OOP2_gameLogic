#pragma once

#include "Plugin/IPlugin.h"
#include <string>
#include <memory>
#include <functional>
#include <exception>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

class GameSession;

/**
 * @brief Sandbox for isolating plugin execution
 * 
 * This class provides isolation for plugin execution to prevent
 * plugin crashes from affecting the main game.
 */
class PluginSandbox {
public:
    /**
     * @brief Constructor
     * @param plugin Plugin to sandbox
     * @param name Plugin name
     */
    PluginSandbox(std::unique_ptr<IPlugin> plugin, const std::string& name);
    
    /**
     * @brief Destructor
     */
    ~PluginSandbox();
    
    /**
     * @brief Initialize the plugin in the sandbox
     * @param gameSession Game session
     * @return True if initialization succeeded
     */
    bool initialize(GameSession* gameSession);
    
    /**
     * @brief Update the plugin in the sandbox
     * @param deltaTime Time since last frame
     * @return True if update succeeded
     */
    bool update(float deltaTime);
    
    /**
     * @brief Render the plugin in the sandbox
     * @param window Render window
     * @return True if render succeeded
     */
    bool render(sf::RenderWindow& window);
    
    /**
     * @brief Shutdown the plugin in the sandbox
     * @return True if shutdown succeeded
     */
    bool shutdown();
    
    /**
     * @brief Get the plugin name
     * @return Plugin name
     */
    const std::string& getName() const { return m_name; }
    
    /**
     * @brief Get the plugin
     * @return Plugin pointer
     */
    IPlugin* getPlugin() { return m_plugin.get(); }
    
    /**
     * @brief Check if plugin is in error state
     * @return True if plugin has encountered an error
     */
    bool hasError() const { return m_hasError; }
    
    /**
     * @brief Get the last error message
     * @return Error message
     */
    std::string getLastError() const { return m_lastError; }
    
    /**
     * @brief Set error callback
     * @param callback Function to call when an error occurs
     */
    void setErrorCallback(std::function<void(const std::string&, const std::string&)> callback) {
        m_errorCallback = callback;
    }
    
    /**
     * @brief Reset error state
     */
    void resetError() {
        m_hasError = false;
        m_lastError.clear();
    }
    
private:
    // Try-catch wrapper for plugin operations
    template<typename Func>
    bool trySafely(const std::string& operation, Func&& func);
    
    // Report error
    void reportError(const std::string& operation, const std::string& error);
    
    std::unique_ptr<IPlugin> m_plugin;
    std::string m_name;
    std::atomic<bool> m_hasError{false};
    std::string m_lastError;
    std::function<void(const std::string&, const std::string&)> m_errorCallback;
};