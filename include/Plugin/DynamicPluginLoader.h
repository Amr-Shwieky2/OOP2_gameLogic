#pragma once

#include "Plugin/IPlugin.h"
#include "Plugin/PluginFactory.h"
#include <string>
#include <memory>
#include <filesystem>

/**
 * @brief Dynamic library handle wrapper
 */
class DynamicLibrary {
public:
    /**
     * @brief Load a dynamic library
     * @param path Path to the library file
     * @return True if successful
     */
    bool load(const std::string& path);
    
    /**
     * @brief Unload the dynamic library
     */
    void unload();
    
    /**
     * @brief Get function address from the library
     * @param name Name of the function
     * @return Function pointer or nullptr if not found
     */
    void* getFunction(const std::string& name);
    
    /**
     * @brief Check if library is loaded
     * @return True if loaded
     */
    bool isLoaded() const;
    
    /**
     * @brief Get the last error message
     * @return Error message string
     */
    std::string getLastError() const;
    
    /**
     * @brief Destructor - unloads library if loaded
     */
    ~DynamicLibrary();
    
private:
    void* m_handle = nullptr;
    std::string m_path;
    std::string m_lastError;
};

/**
 * @brief Function type for creating plugin factory
 */
using CreatePluginFactoryFunc = IPluginFactory* (*)();

/**
 * @brief Function type for destroying plugin factory
 */
using DestroyPluginFactoryFunc = void (*)(IPluginFactory*);

/**
 * @brief Dynamic plugin loader
 * 
 * This class loads plugins from dynamic libraries (.dll, .so, .dylib)
 */
class DynamicPluginLoader {
public:
    /**
     * @brief Load a plugin from a dynamic library
     * @param path Path to the library file
     * @return Unique pointer to the plugin factory or nullptr if failed
     */
    static std::unique_ptr<IPluginFactory> loadPluginFactory(const std::string& path);
    
    /**
     * @brief Discover plugin libraries in a directory
     * @param directory Directory to search
     * @param extension File extension to look for
     * @return Vector of plugin library paths
     */
    static std::vector<std::string> discoverPluginLibraries(
        const std::filesystem::path& directory,
        const std::string& extension);
    
private:
    // Helper to get correct dynamic library extension for current platform
    static std::string getDefaultLibraryExtension();
};