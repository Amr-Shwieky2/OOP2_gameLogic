#include "Plugin/PluginManager.h"
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>

PluginManager& PluginManager::getInstance() {
    static PluginManager instance;
    return instance;
}

bool PluginManager::initialize(const PluginVersion& gameVersion, GameSession* gameSession) {
    if (m_initialized) {
        return true; // Already initialized
    }
    
    m_gameVersion = gameVersion;
    m_gameSession = gameSession;
    m_initialized = true;
    
    std::cout << "Plugin Manager initialized with game version " << gameVersion << std::endl;
    return true;
}

bool PluginManager::shutdown() {
    if (!m_initialized) {
        return true; // Already shut down
    }
    
    // Unload plugins in reverse load order
    for (auto it = m_loadOrder.rbegin(); it != m_loadOrder.rend(); ++it) {
        const std::string& pluginName = *it;
        auto pluginIt = m_loadedPlugins.find(pluginName);
        if (pluginIt != m_loadedPlugins.end()) {
            if (!pluginIt->second->shutdown()) {
                reportError(pluginName, "Failed to shut down plugin");
                // Continue shutting down other plugins
            }
        }
    }
    
    m_loadedPlugins.clear();
    m_loadOrder.clear();
    m_initialized = false;
    std::cout << "Plugin Manager shut down" << std::endl;
    return true;
}

void PluginManager::update(float deltaTime) {
    if (!m_initialized) return;
    
    for (const auto& pluginName : m_loadOrder) {
        auto it = m_loadedPlugins.find(pluginName);
        if (it != m_loadedPlugins.end()) {
            // Update each plugin in dependency order
            it->second->update(deltaTime);
        }
    }
}

void PluginManager::render(sf::RenderWindow& window) {
    if (!m_initialized) return;
    
    for (const auto& pluginName : m_loadOrder) {
        auto it = m_loadedPlugins.find(pluginName);
        if (it != m_loadedPlugins.end()) {
            // Render each plugin in dependency order
            it->second->render(window);
        }
    }
}

bool PluginManager::registerPluginFactory(std::unique_ptr<IPluginFactory> factory) {
    if (!factory) {
        reportError("Unknown", "Null plugin factory provided");
        return false;
    }
    
    std::string pluginName = factory->getPluginName();
    
    // Check if already registered
    if (m_factories.find(pluginName) != m_factories.end()) {
        reportError(pluginName, "Plugin already registered");
        return false;
    }
    
    // Check compatibility
    if (!factory->isPluginCompatible(m_gameVersion)) {
        reportError(pluginName, "Plugin is not compatible with game version " + m_gameVersion.toString());
        return false;
    }
    
    m_factories[pluginName] = std::move(factory);
    std::cout << "Registered plugin: " << pluginName << std::endl;
    return true;
}

bool PluginManager::loadPlugin(const std::string& pluginName) {
    // Check if already loaded
    if (isPluginLoaded(pluginName)) {
        std::cout << "Plugin already loaded: " << pluginName << std::endl;
        return true;
    }
    
    // Find factory
    auto factoryIt = m_factories.find(pluginName);
    if (factoryIt == m_factories.end()) {
        reportError(pluginName, "Plugin factory not found");
        return false;
    }
    
    // Load dependencies first
    if (!loadPluginDependencies(pluginName)) {
        reportError(pluginName, "Failed to load plugin dependencies");
        return false;
    }
    
    // Create plugin instance
    std::unique_ptr<IPlugin> plugin = factoryIt->second->createPlugin();
    if (!plugin) {
        reportError(pluginName, "Failed to create plugin instance");
        return false;
    }
    
    // Initialize plugin
    if (!plugin->initialize(m_gameSession)) {
        reportError(pluginName, "Failed to initialize plugin");
        return false;
    }
    
    // Store plugin
    m_loadedPlugins[pluginName] = std::move(plugin);
    m_loadOrder.push_back(pluginName);
    
    std::cout << "Loaded plugin: " << pluginName << std::endl;
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    auto it = m_loadedPlugins.find(pluginName);
    if (it == m_loadedPlugins.end()) {
        // Plugin not loaded
        return true;
    }
    
    // Check if other loaded plugins depend on this one
    for (const auto& [otherName, otherPlugin] : m_loadedPlugins) {
        if (otherName == pluginName) continue;
        
        const auto& dependencies = otherPlugin->getDependencies();
        if (std::find(dependencies.begin(), dependencies.end(), pluginName) != dependencies.end()) {
            reportError(pluginName, "Cannot unload plugin: it is a dependency of " + otherName);
            return false;
        }
    }
    
    // Shutdown the plugin
    if (!it->second->shutdown()) {
        reportError(pluginName, "Failed to shut down plugin");
        return false;
    }
    
    // Remove from loaded plugins
    m_loadedPlugins.erase(it);
    
    // Remove from load order
    auto orderIt = std::find(m_loadOrder.begin(), m_loadOrder.end(), pluginName);
    if (orderIt != m_loadOrder.end()) {
        m_loadOrder.erase(orderIt);
    }
    
    std::cout << "Unloaded plugin: " << pluginName << std::endl;
    return true;
}

bool PluginManager::reloadPlugin(const std::string& pluginName) {
    if (!isPluginLoaded(pluginName)) {
        reportError(pluginName, "Cannot reload plugin: it is not loaded");
        return false;
    }
    
    // Check if plugin supports hot reload
    IPlugin* plugin = getPlugin(pluginName);
    if (!plugin->supportsHotReload()) {
        reportError(pluginName, "Plugin does not support hot reload");
        return false;
    }
    
    // Unload and load plugin
    if (!unloadPlugin(pluginName)) {
        reportError(pluginName, "Failed to unload plugin for reload");
        return false;
    }
    
    if (!loadPlugin(pluginName)) {
        reportError(pluginName, "Failed to reload plugin");
        return false;
    }
    
    std::cout << "Reloaded plugin: " << pluginName << std::endl;
    return true;
}

int PluginManager::discoverAndLoadPlugins(const std::filesystem::path& directory) {
    int count = 0;
    
    // In a real implementation, this would scan the directory for plugin DLLs/SOs
    // and dynamically load them. For this example, we'll just load registered plugins.
    
    // Load all registered plugins that aren't already loaded
    std::vector<std::string> availablePlugins = getAvailablePluginNames();
    for (const auto& pluginName : availablePlugins) {
        if (!isPluginLoaded(pluginName)) {
            if (loadPlugin(pluginName)) {
                count++;
            }
        }
    }
    
    // Sort plugins topologically after loading
    sortPluginsTopologically();
    
    std::cout << "Discovered and loaded " << count << " plugins from " << directory << std::endl;
    return count;
}

IPlugin* PluginManager::getPlugin(const std::string& pluginName) {
    auto it = m_loadedPlugins.find(pluginName);
    return (it != m_loadedPlugins.end()) ? it->second.get() : nullptr;
}

std::vector<IPlugin*> PluginManager::getLoadedPlugins() const {
    std::vector<IPlugin*> plugins;
    plugins.reserve(m_loadedPlugins.size());
    
    for (const auto& [name, plugin] : m_loadedPlugins) {
        plugins.push_back(plugin.get());
    }
    
    return plugins;
}

std::vector<std::string> PluginManager::getAvailablePluginNames() const {
    std::vector<std::string> names;
    names.reserve(m_factories.size());
    
    for (const auto& [name, factory] : m_factories) {
        names.push_back(name);
    }
    
    return names;
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    return m_loadedPlugins.find(pluginName) != m_loadedPlugins.end();
}

bool PluginManager::loadPluginDependencies(const std::string& pluginName) {
    auto factoryIt = m_factories.find(pluginName);
    if (factoryIt == m_factories.end()) {
        reportError(pluginName, "Plugin factory not found when loading dependencies");
        return false;
    }
    
    // Create temporary plugin instance to get dependencies
    auto tempPlugin = factoryIt->second->createPlugin();
    if (!tempPlugin) {
        reportError(pluginName, "Failed to create temporary plugin instance for dependency check");
        return false;
    }
    
    // Get plugin dependencies
    std::vector<std::string> dependencies = tempPlugin->getDependencies();
    
    // Load each dependency
    for (const auto& dependency : dependencies) {
        if (!loadPlugin(dependency)) {
            reportError(pluginName, "Failed to load dependency: " + dependency);
            return false;
        }
    }
    
    return true;
}

void PluginManager::reportError(const std::string& pluginName, const std::string& error) {
    std::cerr << "Plugin error [" << pluginName << "]: " << error << std::endl;
    
    if (m_errorCallback) {
        m_errorCallback(pluginName, error);
    }
}

void PluginManager::sortPluginsTopologically() {
    // Clear current load order
    m_loadOrder.clear();
    
    // Build dependency graph
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, int> inDegree;
    
    // Initialize graph
    for (const auto& [name, plugin] : m_loadedPlugins) {
        graph[name] = std::vector<std::string>();
        inDegree[name] = 0;
    }
    
    // Build graph edges and in-degrees
    for (const auto& [name, plugin] : m_loadedPlugins) {
        const auto& dependencies = plugin->getDependencies();
        for (const auto& dependency : dependencies) {
            // Only consider loaded dependencies
            if (m_loadedPlugins.find(dependency) != m_loadedPlugins.end()) {
                graph[dependency].push_back(name); // Reverse edge for topological sort
                inDegree[name]++;
            }
        }
    }
    
    // Perform topological sort
    std::queue<std::string> queue;
    
    // Add all nodes with in-degree 0 (no dependencies)
    for (const auto& [name, degree] : inDegree) {
        if (degree == 0) {
            queue.push(name);
        }
    }
    
    // Process queue
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop();
        
        m_loadOrder.push_back(current);
        
        // Update in-degrees
        for (const auto& dependent : graph[current]) {
            inDegree[dependent]--;
            if (inDegree[dependent] == 0) {
                queue.push(dependent);
            }
        }
    }
    
    // Check for circular dependencies
    if (m_loadOrder.size() != m_loadedPlugins.size()) {
        std::cerr << "Warning: Circular dependencies detected in plugins!" << std::endl;
        
        // Fall back to arbitrary order
        m_loadOrder.clear();
        for (const auto& [name, plugin] : m_loadedPlugins) {
            m_loadOrder.push_back(name);
        }
    }
}

bool PluginManager::checkCircularDependencies() {
    // Build dependency graph
    std::unordered_map<std::string, std::vector<std::string>> graph;
    
    // Initialize graph
    for (const auto& [name, plugin] : m_loadedPlugins) {
        graph[name] = plugin->getDependencies();
    }
    
    // For each plugin, check if it depends on itself through a cycle
    for (const auto& [start, _] : m_loadedPlugins) {
        std::set<std::string> visited;
        std::set<std::string> recursionStack;
        
        // Define a recursive DFS function for cycle detection
        std::function<bool(const std::string&)> hasCycle = [&](const std::string& current) {
            if (recursionStack.find(current) != recursionStack.end()) {
                return true; // Found a cycle
            }
            
            if (visited.find(current) != visited.end()) {
                return false; // Already checked, no cycle
            }
            
            // Mark as visited and add to recursion stack
            visited.insert(current);
            recursionStack.insert(current);
            
            // Check all dependencies
            for (const auto& dependency : graph[current]) {
                if (m_loadedPlugins.find(dependency) != m_loadedPlugins.end() &&
                    hasCycle(dependency)) {
                    return true;
                }
            }
            
            // Remove from recursion stack
            recursionStack.erase(current);
            return false;
        };
        
        // Check for cycles starting from this plugin
        if (hasCycle(start)) {
            return true; // Found a circular dependency
        }
    }
    
    return false; // No circular dependencies
}