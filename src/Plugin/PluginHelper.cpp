#include "Plugin/PluginHelper.h"
#include "Plugin/PluginManager.h"
#include <iostream>

PluginHelper::PluginHelper(const std::string& pluginName, GameSession* gameSession)
    : m_pluginName(pluginName), m_gameSession(gameSession)
{
    // Log creation
    std::cout << "PluginHelper created for plugin: " << pluginName << std::endl;
}

PluginHelper::~PluginHelper() {
    // Nothing specific to clean up
}

void PluginHelper::sendEvent(const std::string& eventName, const std::string& eventData) {
    PluginCustomEvent event(m_pluginName, eventName, eventData);
    EventSystem::getInstance().publish(event);
}

void PluginHelper::listenForEvent(const std::string& eventName, 
                                std::function<void(const PluginCustomEvent&)> handler) {
    EventSystem::getInstance().subscribe<PluginCustomEvent>(
        [eventName, handler](const PluginCustomEvent& event) {
            if (event.getEventName() == eventName) {
                handler(event);
            }
        }
    );
}

void PluginHelper::listenForPluginLoaded(std::function<void(const PluginLoadedEvent&)> handler) {
    EventSystem::getInstance().subscribe<PluginLoadedEvent>(handler);
}

void PluginHelper::listenForPluginUnloaded(std::function<void(const PluginUnloadedEvent&)> handler) {
    EventSystem::getInstance().subscribe<PluginUnloadedEvent>(handler);
}

void PluginHelper::log(const std::string& message) {
    std::cout << "[Plugin: " << m_pluginName << "] " << message << std::endl;
}

void PluginHelper::logError(const std::string& error) {
    std::cerr << "[Plugin: " << m_pluginName << "] ERROR: " << error << std::endl;
}

bool PluginHelper::isPluginLoaded(const std::string& pluginName) {
    return PluginManager::getInstance().isPluginLoaded(pluginName);
}

bool PluginHelper::requestPluginLoad(const std::string& pluginName) {
    return PluginManager::getInstance().loadPlugin(pluginName);
}

bool PluginHelper::requestPluginUnload(const std::string& pluginName) {
    return PluginManager::getInstance().unloadPlugin(pluginName);
}

bool PluginHelper::requestPluginReload(const std::string& pluginName) {
    return PluginManager::getInstance().reloadPlugin(pluginName);
}