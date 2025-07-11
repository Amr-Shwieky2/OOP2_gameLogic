#pragma once

#include "Scripting/IScriptEngine.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <any>
#include <optional>
#include <mutex>

namespace Scripting {

/**
 * @brief Script file change event data
 */
struct ScriptFileChangeEvent {
    std::string filename;         ///< Script filename that changed
    bool isNew = false;           ///< Whether the file is new
    bool isDeleted = false;       ///< Whether the file was deleted
    bool isModified = false;      ///< Whether the file was modified
};

/**
 * @brief Script file change callback
 */
using ScriptFileChangeCallback = std::function<void(const ScriptFileChangeEvent&)>;

/**
 * @brief Script manager for the game
 * 
 * This class manages script engines, script execution, and script hot-reloading.
 */
class ScriptManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the script manager
     */
    static ScriptManager& getInstance();
    
    /**
     * @brief Initialize the script manager
     * @param gameSession Pointer to the current game session
     * @return True if initialization succeeds
     */
    bool initialize(GameSession* gameSession);
    
    /**
     * @brief Shutdown the script manager
     * @return True if shutdown succeeds
     */
    bool shutdown();
    
    /**
     * @brief Update script system
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Create a script engine for a specific language
     * @param language Script language to create
     * @return Pointer to created engine, or nullptr on failure
     */
    IScriptEngine* createEngine(ScriptLanguage language);
    
    /**
     * @brief Get a script engine by language
     * @param language Script language to get
     * @return Pointer to engine, or nullptr if not found
     */
    IScriptEngine* getEngine(ScriptLanguage language);
    
    /**
     * @brief Get the default script engine
     * @return Pointer to default engine, or nullptr if none is set
     */
    IScriptEngine* getDefaultEngine() const { return m_defaultEngine; }
    
    /**
     * @brief Set the default script engine
     * @param language Script language to set as default
     * @return True if engine exists and was set as default
     */
    bool setDefaultEngine(ScriptLanguage language);
    
    /**
     * @brief Execute a script file using the appropriate engine
     * @param filename Path to the script file
     * @return Result of the script execution
     */
    ScriptResult executeScript(const std::string& filename);
    
    /**
     * @brief Execute script code using the default engine
     * @param code Script code as string
     * @param name Optional name for the script
     * @return Result of the script execution
     */
    ScriptResult executeString(const std::string& code, const std::string& name = "unnamed");
    
    /**
     * @brief Call a global function in a script
     * @param functionName Name of the function
     * @param args Arguments to pass to the function
     * @param engine Optional engine to use (default if nullptr)
     * @return Result of the function call
     */
    ScriptResult callFunction(const std::string& functionName,
                            const std::vector<std::any>& args = {},
                            IScriptEngine* engine = nullptr);
    
    /**
     * @brief Register a global function to be callable from scripts
     * @param name Name of the function in scripts
     * @param function C++ function to bind
     * @param engine Optional engine to register with (default if nullptr)
     * @return True if registration succeeds
     */
    bool registerFunction(const std::string& name,
                         std::function<std::any(const std::vector<std::any>&)> function,
                         IScriptEngine* engine = nullptr);
    
    /**
     * @brief Load all scripts from a directory
     * @param directory Directory to load scripts from
     * @param recursive Whether to search recursively
     * @return Number of scripts loaded
     */
    int loadScriptsFromDirectory(const std::filesystem::path& directory, bool recursive = true);
    
    /**
     * @brief Enable hot-reloading of scripts
     * @param enabled Whether hot-reloading should be enabled
     * @param pollIntervalSeconds Interval between file checks in seconds
     */
    void enableHotReload(bool enabled, float pollIntervalSeconds = 1.0f);
    
    /**
     * @brief Check if hot-reloading is enabled
     * @return True if hot-reloading is enabled
     */
    bool isHotReloadEnabled() const { return m_hotReloadEnabled; }
    
    /**
     * @brief Register a callback for script file changes
     * @param callback Function to call when a script file changes
     * @return ID to use for unregistering the callback
     */
    int registerFileChangeCallback(ScriptFileChangeCallback callback);
    
    /**
     * @brief Unregister a file change callback
     * @param callbackId ID returned from registerFileChangeCallback
     * @return True if callback was unregistered
     */
    bool unregisterFileChangeCallback(int callbackId);
    
    /**
     * @brief Add a script directory to watch for changes
     * @param directory Directory path to watch
     * @param recursive Whether to watch recursively
     * @return True if directory was added
     */
    bool addScriptDirectory(const std::filesystem::path& directory, bool recursive = true);
    
    /**
     * @brief Remove a script directory from watching
     * @param directory Directory path to remove
     * @return True if directory was removed
     */
    bool removeScriptDirectory(const std::filesystem::path& directory);
    
    /**
     * @brief Get all watched script directories
     * @return Vector of watched directories
     */
    std::vector<std::filesystem::path> getScriptDirectories() const;
    
    /**
     * @brief Enable script debugging
     * @param enabled Whether debugging should be enabled
     */
    void enableDebugging(bool enabled);
    
    /**
     * @brief Check if script debugging is enabled
     * @return True if debugging is enabled
     */
    bool isDebuggingEnabled() const { return m_debuggingEnabled; }
    
    /**
     * @brief Add a breakpoint to a script
     * @param breakpoint Breakpoint information
     * @return True if breakpoint was added
     */
    bool addBreakpoint(const Breakpoint& breakpoint);
    
    /**
     * @brief Remove a breakpoint
     * @param scriptName Script file name
     * @param lineNumber Line number of the breakpoint
     * @return True if breakpoint was removed
     */
    bool removeBreakpoint(const std::string& scriptName, int lineNumber);
    
    /**
     * @brief Set breakpoint callback
     * @param callback Function to call when a breakpoint is hit
     */
    void setBreakpointCallback(BreakpointCallback callback);
    
    /**
     * @brief Continue execution after a breakpoint
     * @return True if execution continues
     */
    bool continueExecution();
    
    /**
     * @brief Get the script language for a file extension
     * @param extension File extension (e.g., ".lua", ".js")
     * @return Script language enum value
     */
    static ScriptLanguage getLanguageForExtension(const std::string& extension);
    
    /**
     * @brief Get file extension for a script language
     * @param language Script language enum value
     * @return File extension string
     */
    static std::string getExtensionForLanguage(ScriptLanguage language);
    
    /**
     * @brief Get profiling results for all engines
     * @return Map of engine name to profiling results
     */
    std::map<std::string, std::unordered_map<std::string, double>> getProfilingResults() const;
    
    /**
     * @brief Reset profiling data for all engines
     */
    void resetProfilingData();
    
    /**
     * @brief Enable profiling for all engines
     * @param enabled Whether profiling should be enabled
     */
    void enableProfiling(bool enabled);
    
    /**
     * @brief Enable sandbox mode for all engines
     * @param enabled Whether sandbox mode should be enabled
     */
    void enableSandboxMode(bool enabled);
    
    /**
     * @brief Check if sandbox mode is enabled
     * @return True if sandbox mode is enabled
     */
    bool isSandboxModeEnabled() const { return m_sandboxEnabled; }
    
    /**
     * @brief Validate a script without executing it
     * @param scriptCode Script code as string
     * @param language Script language to use
     * @return True if script is valid
     */
    bool validateScript(const std::string& scriptCode, ScriptLanguage language);
    
    /**
     * @brief Set error handler for script exceptions
     * @param handler Function to call when a script error occurs
     */
    void setErrorHandler(std::function<void(const std::string&, int, const std::string&)> handler);
    
private:
    // Private constructor for singleton
    ScriptManager() = default;
    ~ScriptManager() = default;
    
    // Prevent copying or moving
    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;
    ScriptManager(ScriptManager&&) = delete;
    ScriptManager& operator=(ScriptManager&&) = delete;
    
    /**
     * @brief Check for file changes in watched directories
     */
    void checkForFileChanges();
    
    /**
     * @brief Reload changed scripts
     * @param changedFiles Vector of changed file paths
     */
    void reloadChangedScripts(const std::vector<std::filesystem::path>& changedFiles);
    
    /**
     * @brief Initialize a script engine
     * @param engine Engine to initialize
     * @return True if initialization succeeds
     */
    bool initializeEngine(IScriptEngine* engine);
    
    // Member variables
    GameSession* m_gameSession = nullptr;
    std::map<ScriptLanguage, std::unique_ptr<IScriptEngine>> m_engines;
    IScriptEngine* m_defaultEngine = nullptr;
    bool m_initialized = false;
    
    // Hot-reloading
    bool m_hotReloadEnabled = false;
    float m_pollIntervalSeconds = 1.0f;
    float m_timeSinceLastPoll = 0.0f;
    std::vector<std::filesystem::path> m_scriptDirectories;
    std::map<std::filesystem::path, std::filesystem::file_time_type> m_fileTimestamps;
    std::map<int, ScriptFileChangeCallback> m_fileChangeCallbacks;
    int m_nextCallbackId = 1;
    
    // Debugging
    bool m_debuggingEnabled = false;
    BreakpointCallback m_breakpointCallback;
    
    // Profiling
    bool m_profilingEnabled = false;
    
    // Sandbox mode
    bool m_sandboxEnabled = false;
    
    // Error handling
    std::function<void(const std::string&, int, const std::string&)> m_errorHandler;
    
    // Thread safety
    mutable std::mutex m_mutex;
    
    // Loaded scripts
    std::unordered_map<std::string, ScriptLanguage> m_loadedScripts;
};

} // namespace Scripting