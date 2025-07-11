#pragma once

#include "EventSystem.h"
#include "Plugin/PluginEvents.h"
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>
#include <typeindex>

class GameSession;

/**
 * @brief Helper class for plugins
 * 
 * This class provides common functionality for plugins such as
 * event handling and communication with other plugins.
 */
class PluginHelper {
public:
    /**
     * @brief Constructor
     * @param pluginName Plugin name
     * @param gameSession Game session
     */
    PluginHelper(const std::string& pluginName, GameSession* gameSession);
    
    /**
     * @brief Destructor
     */
    ~PluginHelper();
    
    /**
     * @brief Get the plugin name
     * @return Plugin name
     */
    const std::string& getPluginName() const { return m_pluginName; }
    
    /**
     * @brief Get the game session
     * @return Game session
     */
    GameSession* getGameSession() const { return m_gameSession; }
    
    /**
     * @brief Send a custom event to other plugins
     * @param eventName Event name
     * @param eventData Event data
     */
    void sendEvent(const std::string& eventName, const std::string& eventData = "");
    
    /**
     * @brief Listen for plugin events
     * @param eventName Event name
     * @param handler Event handler
     */
    void listenForEvent(const std::string& eventName, 
                      std::function<void(const PluginCustomEvent&)> handler);
    
    /**
     * @brief Listen for plugin loaded events
     * @param handler Event handler
     */
    void listenForPluginLoaded(std::function<void(const PluginLoadedEvent&)> handler);
    
    /**
     * @brief Listen for plugin unloaded events
     * @param handler Event handler
     */
    void listenForPluginUnloaded(std::function<void(const PluginUnloadedEvent&)> handler);
    
    /**
     * @brief Listen for any game event
     * @tparam EventType Event type
     * @param handler Event handler
     */
    template<typename EventType>
    void listenForGameEvent(std::function<void(const EventType&)> handler) {
        static_assert(std::is_base_of<Event, EventType>::value, "EventType must derive from Event");
        
        EventSystem::getInstance().subscribe<EventType>(handler);
    }
    
    /**
     * @brief Log a message
     * @param message Message to log
     */
    void log(const std::string& message);
    
    /**
     * @brief Log an error
     * @param error Error to log
     */
    void logError(const std::string& error);
    
    /**
     * @brief Check if another plugin is loaded
     * @param pluginName Plugin name to check
     * @return True if plugin is loaded
     */
    bool isPluginLoaded(const std::string& pluginName);
    
    /**
     * @brief Request a plugin to be loaded
     * @param pluginName Plugin name to load
     * @return True if request was successful
     */
    bool requestPluginLoad(const std::string& pluginName);
    
    /**
     * @brief Request a plugin to be unloaded
     * @param pluginName Plugin name to unload
     * @return True if request was successful
     */
    bool requestPluginUnload(const std::string& pluginName);
    
    /**
     * @brief Request a plugin to be reloaded
     * @param pluginName Plugin name to reload
     * @return True if request was successful
     */
    bool requestPluginReload(const std::string& pluginName);
    
private:
    std::string m_pluginName;
    GameSession* m_gameSession;
};