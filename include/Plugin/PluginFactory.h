#pragma once

#include "Plugin/IPlugin.h"
#include <memory>
#include <string>

/**
 * @brief Interface for plugin factories
 * 
 * This class defines the interface for plugin factories that create plugin instances.
 */
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    
    /**
     * @brief Create a new plugin instance
     * @return Unique pointer to the created plugin
     */
    virtual std::unique_ptr<IPlugin> createPlugin() = 0;
    
    /**
     * @brief Get the plugin name
     * @return The plugin's unique name
     */
    virtual std::string getPluginName() const = 0;
    
    /**
     * @brief Get the plugin version
     * @return The plugin's version
     */
    virtual PluginVersion getPluginVersion() const = 0;
    
    /**
     * @brief Check if factory can create plugin compatible with game version
     * @param gameVersion The current game version
     * @return True if compatible, false otherwise
     */
    virtual bool isPluginCompatible(const PluginVersion& gameVersion) const = 0;
};

/**
 * @brief Template implementation of IPluginFactory for specific plugin types
 */
template<typename PluginType>
class PluginFactory : public IPluginFactory {
public:
    /**
     * @brief Create a new instance of the plugin
     * @return Unique pointer to the created plugin
     */
    std::unique_ptr<IPlugin> createPlugin() override {
        return std::make_unique<PluginType>();
    }
    
    /**
     * @brief Get the plugin name
     * @return The plugin's unique name
     */
    std::string getPluginName() const override {
        // Create temporary instance to get name
        PluginType tempInstance;
        return tempInstance.getName();
    }
    
    /**
     * @brief Get the plugin version
     * @return The plugin's version
     */
    PluginVersion getPluginVersion() const override {
        // Create temporary instance to get version
        PluginType tempInstance;
        return tempInstance.getVersion();
    }
    
    /**
     * @brief Check if factory can create plugin compatible with game version
     * @param gameVersion The current game version
     * @return True if compatible, false otherwise
     */
    bool isPluginCompatible(const PluginVersion& gameVersion) const override {
        // Create temporary instance to check compatibility
        PluginType tempInstance;
        return tempInstance.isCompatible(gameVersion);
    }
};