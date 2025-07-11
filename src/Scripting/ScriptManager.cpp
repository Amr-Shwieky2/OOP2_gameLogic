#include "Scripting/ScriptManager.h"
#include "Scripting/LuaScriptEngine.h"
#include "GameSession.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace Scripting {

// Singleton instance
ScriptManager& ScriptManager::getInstance() {
    static ScriptManager instance;
    return instance;
}

// Initialize the script manager
bool ScriptManager::initialize(GameSession* gameSession) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_initialized) {
        return true;
    }
    
    m_gameSession = gameSession;
    m_initialized = true;
    
    // Create default script engines
    createEngine(ScriptLanguage::Lua);
    
    // Set default engine
    setDefaultEngine(ScriptLanguage::Lua);
    
    return true;
}

// Shutdown the script manager
bool ScriptManager::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return true;
    }
    
    // Shutdown all engines
    for (auto& [language, engine] : m_engines) {
        engine->shutdown();
    }
    
    // Clear engines
    m_engines.clear();
    m_defaultEngine = nullptr;
    
    m_initialized = false;
    return true;
}

// Update script system
void ScriptManager::update(float deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    // Check for file changes if hot reload is enabled
    if (m_hotReloadEnabled) {
        m_timeSinceLastPoll += deltaTime;
        
        if (m_timeSinceLastPoll >= m_pollIntervalSeconds) {
            checkForFileChanges();
            m_timeSinceLastPoll = 0.0f;
        }
    }
}

// Create a script engine for a specific language
IScriptEngine* ScriptManager::createEngine(ScriptLanguage language) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if engine already exists
    auto it = m_engines.find(language);
    if (it != m_engines.end()) {
        return it->second.get();
    }
    
    // Create new engine based on language
    std::unique_ptr<IScriptEngine> engine;
    
    switch (language) {
        case ScriptLanguage::Lua:
            engine = std::make_unique<LuaScriptEngine>();
            break;
            
        case ScriptLanguage::JavaScript:
            // JavaScript engine not implemented yet
            std::cerr << "JavaScript engine not implemented yet" << std::endl;
            return nullptr;
            
        case ScriptLanguage::Python:
            // Python engine not implemented yet
            std::cerr << "Python engine not implemented yet" << std::endl;
            return nullptr;
            
        default:
            std::cerr << "Unknown script language" << std::endl;
            return nullptr;
    }
    
    // Initialize the engine
    if (engine && initializeEngine(engine.get())) {
        auto* rawPtr = engine.get();
        m_engines[language] = std::move(engine);
        return rawPtr;
    }
    
    return nullptr;
}

// Get a script engine by language
IScriptEngine* ScriptManager::getEngine(ScriptLanguage language) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_engines.find(language);
    if (it != m_engines.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

// Set the default script engine
bool ScriptManager::setDefaultEngine(ScriptLanguage language) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_engines.find(language);
    if (it != m_engines.end()) {
        m_defaultEngine = it->second.get();
        return true;
    }
    
    return false;
}

// Execute a script file using the appropriate engine
ScriptResult ScriptManager::executeScript(const std::string& filename) {
    // Get the file extension to determine the language
    std::string extension = fs::path(filename).extension().string();
    ScriptLanguage language = getLanguageForExtension(extension);
    
    // Get the appropriate engine
    IScriptEngine* engine = getEngine(language);
    if (!engine) {
        ScriptResult result;
        result.success = false;
        result.error = "No engine available for " + extension + " scripts";
        return result;
    }
    
    // Execute the script
    return engine->executeFile(filename);
}

// Execute script code using the default engine
ScriptResult ScriptManager::executeString(const std::string& code, const std::string& name) {
    if (!m_defaultEngine) {
        ScriptResult result;
        result.success = false;
        result.error = "No default script engine set";
        return result;
    }
    
    return m_defaultEngine->executeString(code, name);
}

// Call a global function in a script
ScriptResult ScriptManager::callFunction(const std::string& functionName,
                                       const std::vector<std::any>& args,
                                       IScriptEngine* engine) {
    // Use specified engine or default
    IScriptEngine* scriptEngine = engine ? engine : m_defaultEngine;
    
    if (!scriptEngine) {
        ScriptResult result;
        result.success = false;
        result.error = "No script engine available";
        return result;
    }
    
    return scriptEngine->callFunction(functionName, args);
}

// Register a global function to be callable from scripts
bool ScriptManager::registerFunction(const std::string& name,
                                   std::function<std::any(const std::vector<std::any>&)> function,
                                   IScriptEngine* engine) {
    // Use specified engine or default
    IScriptEngine* scriptEngine = engine ? engine : m_defaultEngine;
    
    if (!scriptEngine) {
        return false;
    }
    
    return scriptEngine->registerFunction(name, function);
}

// Load all scripts from a directory
int ScriptManager::loadScriptsFromDirectory(const std::filesystem::path& directory, bool recursive) {
    int count = 0;
    
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return 0;
    }
    
    // Recursion mode
    auto options = recursive ? fs::directory_options::follow_directory_symlink : fs::directory_options::none;
    
    // Iterate through directory
    for (const auto& entry : fs::directory_iterator(directory, options)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            ScriptLanguage language = getLanguageForExtension(extension);
            
            // Skip unsupported file types
            if (language == ScriptLanguage::Custom) {
                continue;
            }
            
            // Execute the script
            ScriptResult result = executeScript(entry.path().string());
            if (result.success) {
                // Store the language for this script
                m_loadedScripts[entry.path().string()] = language;
                count++;
            }
            else {
                std::cerr << "Failed to load script " << entry.path().string()
                          << ": " << result.error << std::endl;
            }
        }
        else if (recursive && entry.is_directory()) {
            // Recurse into subdirectory
            count += loadScriptsFromDirectory(entry.path(), true);
        }
    }
    
    return count;
}

// Enable hot-reloading of scripts
void ScriptManager::enableHotReload(bool enabled, float pollIntervalSeconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_hotReloadEnabled = enabled;
    m_pollIntervalSeconds = std::max(0.1f, pollIntervalSeconds);
    m_timeSinceLastPoll = 0.0f;
}

// Register a callback for script file changes
int ScriptManager::registerFileChangeCallback(ScriptFileChangeCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    int id = m_nextCallbackId++;
    m_fileChangeCallbacks[id] = callback;
    return id;
}

// Unregister a file change callback
bool ScriptManager::unregisterFileChangeCallback(int callbackId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_fileChangeCallbacks.find(callbackId);
    if (it != m_fileChangeCallbacks.end()) {
        m_fileChangeCallbacks.erase(it);
        return true;
    }
    
    return false;
}

// Add a script directory to watch for changes
bool ScriptManager::addScriptDirectory(const std::filesystem::path& directory, bool recursive) {
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if already watching this directory
    auto it = std::find(m_scriptDirectories.begin(), m_scriptDirectories.end(), directory);
    if (it != m_scriptDirectories.end()) {
        return true;
    }
    
    // Add to watched directories
    m_scriptDirectories.push_back(directory);
    
    // Add all script files in directory to timestamp map
    if (recursive) {
        for (auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                if (getLanguageForExtension(extension) != ScriptLanguage::Custom) {
                    m_fileTimestamps[entry.path()] = fs::last_write_time(entry.path());
                }
            }
        }
    }
    else {
        for (auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                if (getLanguageForExtension(extension) != ScriptLanguage::Custom) {
                    m_fileTimestamps[entry.path()] = fs::last_write_time(entry.path());
                }
            }
        }
    }
    
    return true;
}

// Remove a script directory from watching
bool ScriptManager::removeScriptDirectory(const std::filesystem::path& directory) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find(m_scriptDirectories.begin(), m_scriptDirectories.end(), directory);
    if (it != m_scriptDirectories.end()) {
        m_scriptDirectories.erase(it);
        
        // Remove files in this directory from timestamp map
        for (auto it = m_fileTimestamps.begin(); it != m_fileTimestamps.end();) {
            if (it->first.string().find(directory.string()) == 0) {
                it = m_fileTimestamps.erase(it);
            }
            else {
                ++it;
            }
        }
        
        return true;
    }
    
    return false;
}

// Get all watched script directories
std::vector<std::filesystem::path> ScriptManager::getScriptDirectories() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_scriptDirectories;
}

// Enable script debugging
void ScriptManager::enableDebugging(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_debuggingEnabled = enabled;
    
    // Enable debugging in all engines
    for (auto& [language, engine] : m_engines) {
        // In a real implementation, engines would have debugging support
    }
}

// Add a breakpoint to a script
bool ScriptManager::addBreakpoint(const Breakpoint& breakpoint) {
    // Find the appropriate engine for this script
    for (auto& [language, engine] : m_engines) {
        if (engine->addBreakpoint(breakpoint)) {
            return true;
        }
    }
    
    return false;
}

// Remove a breakpoint
bool ScriptManager::removeBreakpoint(const std::string& scriptName, int lineNumber) {
    // Remove from all engines
    bool removed = false;
    
    for (auto& [language, engine] : m_engines) {
        if (engine->removeBreakpoint(scriptName, lineNumber)) {
            removed = true;
        }
    }
    
    return removed;
}

// Set breakpoint callback
void ScriptManager::setBreakpointCallback(BreakpointCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_breakpointCallback = callback;
    
    // Set callback for all engines
    for (auto& [language, engine] : m_engines) {
        engine->setBreakpointCallback(callback);
    }
}

// Continue execution after a breakpoint
bool ScriptManager::continueExecution() {
    // Continue execution in all engines
    bool result = false;
    
    for (auto& [language, engine] : m_engines) {
        if (engine->continueExecution()) {
            result = true;
        }
    }
    
    return result;
}

// Get the script language for a file extension
ScriptLanguage ScriptManager::getLanguageForExtension(const std::string& extension) {
    if (extension == ".lua") {
        return ScriptLanguage::Lua;
    }
    else if (extension == ".js") {
        return ScriptLanguage::JavaScript;
    }
    else if (extension == ".py") {
        return ScriptLanguage::Python;
    }
    
    return ScriptLanguage::Custom;
}

// Get file extension for a script language
std::string ScriptManager::getExtensionForLanguage(ScriptLanguage language) {
    switch (language) {
        case ScriptLanguage::Lua:
            return ".lua";
        case ScriptLanguage::JavaScript:
            return ".js";
        case ScriptLanguage::Python:
            return ".py";
        default:
            return ".txt";
    }
}

// Get profiling results for all engines
std::map<std::string, std::unordered_map<std::string, double>> ScriptManager::getProfilingResults() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::map<std::string, std::unordered_map<std::string, double>> results;
    
    for (const auto& [language, engine] : m_engines) {
        results[scriptLanguageToString(language)] = engine->getProfilingResults();
    }
    
    return results;
}

// Reset profiling data for all engines
void ScriptManager::resetProfilingData() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& [language, engine] : m_engines) {
        engine->resetProfilingData();
    }
}

// Enable profiling for all engines
void ScriptManager::enableProfiling(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_profilingEnabled = enabled;
    
    for (auto& [language, engine] : m_engines) {
        engine->enableProfiling(enabled);
    }
}

// Enable sandbox mode for all engines
void ScriptManager::enableSandboxMode(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_sandboxEnabled = enabled;
    
    for (auto& [language, engine] : m_engines) {
        engine->setSandboxMode(enabled);
    }
}

// Validate a script without executing it
bool ScriptManager::validateScript(const std::string& scriptCode, ScriptLanguage language) {
    auto engine = getEngine(language);
    if (!engine) {
        return false;
    }
    
    return engine->validateScript(scriptCode).success;
}

// Set error handler for script exceptions
void ScriptManager::setErrorHandler(std::function<void(const std::string&, int, const std::string&)> handler) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_errorHandler = handler;
    
    for (auto& [language, engine] : m_engines) {
        engine->setErrorHandler(handler);
    }
}

// Check for file changes in watched directories
void ScriptManager::checkForFileChanges() {
    std::vector<fs::path> changedFiles;
    
    // Check for new and modified files
    for (const auto& directory : m_scriptDirectories) {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            continue;
        }
        
        try {
            // Check all script files in the directory
            for (auto& entry : fs::recursive_directory_iterator(directory)) {
                if (!entry.is_regular_file()) {
                    continue;
                }
                
                const fs::path& path = entry.path();
                std::string extension = path.extension().string();
                
                // Skip non-script files
                if (getLanguageForExtension(extension) == ScriptLanguage::Custom) {
                    continue;
                }
                
                // Get current write time
                auto currentWriteTime = fs::last_write_time(path);
                
                // Check if this is a new or modified file
                auto it = m_fileTimestamps.find(path);
                if (it == m_fileTimestamps.end()) {
                    // New file
                    m_fileTimestamps[path] = currentWriteTime;
                    changedFiles.push_back(path);
                    
                    // Notify callbacks
                    for (auto& [id, callback] : m_fileChangeCallbacks) {
                        ScriptFileChangeEvent event;
                        event.filename = path.string();
                        event.isNew = true;
                        callback(event);
                    }
                }
                else if (currentWriteTime != it->second) {
                    // Modified file
                    it->second = currentWriteTime;
                    changedFiles.push_back(path);
                    
                    // Notify callbacks
                    for (auto& [id, callback] : m_fileChangeCallbacks) {
                        ScriptFileChangeEvent event;
                        event.filename = path.string();
                        event.isModified = true;
                        callback(event);
                    }
                }
            }
            
            // Check for deleted files
            for (auto it = m_fileTimestamps.begin(); it != m_fileTimestamps.end();) {
                if (!fs::exists(it->first)) {
                    // Notify callbacks
                    for (auto& [id, callback] : m_fileChangeCallbacks) {
                        ScriptFileChangeEvent event;
                        event.filename = it->first.string();
                        event.isDeleted = true;
                        callback(event);
                    }
                    
                    it = m_fileTimestamps.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Error checking directory " << directory << ": " << e.what() << std::endl;
        }
    }
    
    // Reload changed files
    if (!changedFiles.empty()) {
        reloadChangedScripts(changedFiles);
    }
}

// Reload changed scripts
void ScriptManager::reloadChangedScripts(const std::vector<std::filesystem::path>& changedFiles) {
    for (const auto& file : changedFiles) {
        // Get the language for this file
        std::string extension = file.extension().string();
        ScriptLanguage language = getLanguageForExtension(extension);
        
        // Get the appropriate engine
        IScriptEngine* engine = getEngine(language);
        if (!engine) {
            std::cerr << "No engine available for " << extension << " scripts" << std::endl;
            continue;
        }
        
        // Hot reload the script
        if (!engine->hotReloadFile(file.string())) {
            std::cerr << "Failed to reload script " << file << std::endl;
        }
        else {
            std::cout << "Reloaded script " << file << std::endl;
            
            // Update the script language map
            m_loadedScripts[file.string()] = language;
        }
    }
}

// Initialize a script engine
bool ScriptManager::initializeEngine(IScriptEngine* engine) {
    if (!engine || !m_gameSession) {
        return false;
    }
    
    // Initialize the engine
    bool result = engine->initialize(m_gameSession);
    
    if (result) {
        // Set up engine settings
        engine->enableProfiling(m_profilingEnabled);
        engine->setSandboxMode(m_sandboxEnabled);
        if (m_errorHandler) {
            engine->setErrorHandler(m_errorHandler);
        }
        if (m_breakpointCallback) {
            engine->setBreakpointCallback(m_breakpointCallback);
        }
    }
    
    return result;
}

} // namespace Scripting