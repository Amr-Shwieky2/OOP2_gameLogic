#pragma once

#include "EventSystem.h"
#include <string>

/**
 * @brief Base class for plugin events
 */
class PluginEvent : public Event {
public:
    PluginEvent(const std::string& pluginName) : m_pluginName(pluginName) {}
    virtual ~PluginEvent() = default;
    
    const std::string& getPluginName() const { return m_pluginName; }
    
private:
    std::string m_pluginName;
};

/**
 * @brief Event fired when a plugin is loaded
 */
class PluginLoadedEvent : public PluginEvent {
public:
    PluginLoadedEvent(const std::string& pluginName) : PluginEvent(pluginName) {}
    
    const char* getName() const override { return "PluginLoaded"; }
};

/**
 * @brief Event fired when a plugin is unloaded
 */
class PluginUnloadedEvent : public PluginEvent {
public:
    PluginUnloadedEvent(const std::string& pluginName) : PluginEvent(pluginName) {}
    
    const char* getName() const override { return "PluginUnloaded"; }
};

/**
 * @brief Event fired when a plugin is reloaded
 */
class PluginReloadedEvent : public PluginEvent {
public:
    PluginReloadedEvent(const std::string& pluginName) : PluginEvent(pluginName) {}
    
    const char* getName() const override { return "PluginReloaded"; }
};

/**
 * @brief Event fired when a plugin encounters an error
 */
class PluginErrorEvent : public PluginEvent {
public:
    PluginErrorEvent(const std::string& pluginName, const std::string& errorMessage)
        : PluginEvent(pluginName), m_errorMessage(errorMessage) {}
    
    const char* getName() const override { return "PluginError"; }
    const std::string& getErrorMessage() const { return m_errorMessage; }
    
private:
    std::string m_errorMessage;
};

/**
 * @brief Custom event that plugins can use to communicate
 */
class PluginCustomEvent : public PluginEvent {
public:
    PluginCustomEvent(const std::string& pluginName, const std::string& eventName, 
                    const std::string& eventData = "")
        : PluginEvent(pluginName), m_eventName(eventName), m_eventData(eventData) {}
    
    const char* getName() const override { return m_eventName.c_str(); }
    const std::string& getEventName() const { return m_eventName; }
    const std::string& getEventData() const { return m_eventData; }
    
private:
    std::string m_eventName;
    std::string m_eventData;
};