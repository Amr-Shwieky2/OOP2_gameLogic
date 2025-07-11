#pragma once

#include "Plugin/PluginBase.h"
#include "Plugin/PluginFactory.h"

/**
 * @brief Template class for creating plugins
 * 
 * This template provides a simple way to create new plugins
 * with minimal boilerplate code.
 */
template<typename PluginImpl>
class PluginTemplate : public PluginBase {
public:
    /**
     * @brief Constructor that forwards arguments to PluginBase
     */
    template<typename... Args>
    PluginTemplate(Args&&... args) : PluginBase(std::forward<Args>(args)...) {}
    
    /**
     * @brief Initialize plugin
     * @param gameSession Game session
     * @return True if initialization succeeded
     */
    bool initialize(GameSession* gameSession) override {
        if (!PluginBase::initialize(gameSession)) {
            return false;
        }
        
        // Call derived class implementation
        return static_cast<PluginImpl*>(this)->onInitialize();
    }
    
    /**
     * @brief Update plugin
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime) override {
        // Call derived class implementation
        static_cast<PluginImpl*>(this)->onUpdate(deltaTime);
    }
    
    /**
     * @brief Render plugin
     * @param window Render window
     */
    void render(sf::RenderWindow& window) override {
        // Call derived class implementation
        static_cast<PluginImpl*>(this)->onRender(window);
    }
    
    /**
     * @brief Shutdown plugin
     * @return True if shutdown succeeded
     */
    bool shutdown() override {
        // Call derived class implementation
        bool result = static_cast<PluginImpl*>(this)->onShutdown();
        
        // Call base implementation
        return PluginBase::shutdown() && result;
    }
    
protected:
    /**
     * @brief Default implementation of onInitialize (can be overridden)
     * @return True by default
     */
    bool onInitialize() { return true; }
    
    /**
     * @brief Default implementation of onUpdate (can be overridden)
     * @param deltaTime Time since last frame
     */
    void onUpdate(float deltaTime) {}
    
    /**
     * @brief Default implementation of onRender (can be overridden)
     * @param window Render window
     */
    void onRender(sf::RenderWindow& window) {}
    
    /**
     * @brief Default implementation of onShutdown (can be overridden)
     * @return True by default
     */
    bool onShutdown() { return true; }
};

/**
 * @brief Macro to define plugin factory functions for dynamic loading
 * 
 * This macro defines the required extern "C" functions for loading
 * the plugin as a dynamic library.
 */
#define DEFINE_PLUGIN_FACTORY(PluginClass) \
    extern "C" { \
        __declspec(dllexport) IPluginFactory* CreatePluginFactory() { \
            return new PluginFactory<PluginClass>(); \
        } \
        __declspec(dllexport) void DestroyPluginFactory(IPluginFactory* factory) { \
            delete factory; \
        } \
    }