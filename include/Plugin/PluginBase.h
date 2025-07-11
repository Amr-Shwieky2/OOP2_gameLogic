#pragma once

#include "Plugin/IPlugin.h"
#include <functional>

/**
 * @brief Base class for plugins with common implementation
 * 
 * This class provides a default implementation for many of the IPlugin methods,
 * making it easier to create new plugins.
 */
class PluginBase : public IPlugin {
public:
    /**
     * @brief Constructor with basic plugin information
     * 
     * @param name Plugin unique identifier
     * @param version Plugin version
     * @param description Plugin description
     * @param author Plugin author
     * @param category Plugin category
     * @param supportsHotReload Whether plugin supports hot reloading
     */
    PluginBase(
        const std::string& name,
        const PluginVersion& version,
        const std::string& description,
        const std::string& author,
        const std::string& category = "Misc",
        bool supportsHotReload = false
    );
    
    virtual ~PluginBase() = default;

    // IPlugin interface implementations
    std::string getName() const override { return m_name; }
    PluginVersion getVersion() const override { return m_version; }
    std::string getDescription() const override { return m_description; }
    std::string getAuthor() const override { return m_author; }
    std::string getCategory() const override { return m_category; }
    bool supportsHotReload() const override { return m_supportsHotReload; }
    
    // Default implementations for lifecycle methods
    bool initialize(GameSession* gameSession) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    bool shutdown() override;
    
    // Default implementation for compatibility check
    bool isCompatible(const PluginVersion& gameVersion) const override;
    
    // Default implementation for dependencies
    std::vector<std::string> getDependencies() const override;
    
    // Configure update callback
    void setUpdateCallback(std::function<void(float)> callback) { m_updateCallback = callback; }
    
    // Configure render callback
    void setRenderCallback(std::function<void(sf::RenderWindow&)> callback) { m_renderCallback = callback; }
    
    // Add a dependency
    void addDependency(const std::string& pluginName) { m_dependencies.push_back(pluginName); }
    
protected:
    // Access to game session
    GameSession* getGameSession() const { return m_gameSession; }
    
    // Set plugin options
    void setCategory(const std::string& category) { m_category = category; }
    void setSupportsHotReload(bool supports) { m_supportsHotReload = supports; }
    
    // Set custom compatibility checker
    void setCompatibilityChecker(std::function<bool(const PluginVersion&)> checker) {
        m_compatibilityChecker = checker;
    }
    
private:
    std::string m_name;
    PluginVersion m_version;
    std::string m_description;
    std::string m_author;
    std::string m_category;
    bool m_supportsHotReload;
    
    GameSession* m_gameSession = nullptr;
    std::vector<std::string> m_dependencies;
    
    // Function callbacks
    std::function<void(float)> m_updateCallback;
    std::function<void(sf::RenderWindow&)> m_renderCallback;
    std::function<bool(const PluginVersion&)> m_compatibilityChecker;
};