#pragma once

#include <string>
#include <memory>
#include <vector>
#include <any>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <typeindex>

class GameSession;
class Entity;

namespace Scripting {

/**
 * @brief Enumeration of supported script languages
 */
enum class ScriptLanguage {
    Lua,        ///< Lua scripting language
    JavaScript, ///< JavaScript via embedded engine
    Python,     ///< Python via embedded interpreter
    Custom      ///< Custom script language implementation
};

/**
 * @brief Debug information for script execution
 */
struct ScriptDebugInfo {
    std::string scriptName;       ///< Name or path of the script
    int lineNumber = 0;           ///< Current line number
    std::string functionName;     ///< Current function name
    std::string stackTrace;       ///< Full stack trace if available
    std::map<std::string, std::any> localVariables; ///< Local variables at current scope
};

/**
 * @brief Result of script execution
 */
struct ScriptResult {
    bool success = false;         ///< Whether the script executed successfully
    std::string error;            ///< Error message if success is false
    int errorLine = -1;           ///< Line number where the error occurred
    std::optional<std::any> returnValue; ///< Optional return value from script
    double executionTimeMs = 0.0; ///< Execution time in milliseconds
};

/**
 * @brief Breakpoint settings
 */
struct Breakpoint {
    std::string scriptName;       ///< Script file name or identifier
    int lineNumber = 0;           ///< Line to break at
    std::string condition;        ///< Optional condition to evaluate
    bool enabled = true;          ///< Whether the breakpoint is enabled
};

/**
 * @brief Callback for script breakpoints
 * 
 * @param engine The script engine that hit the breakpoint
 * @param debugInfo Debug information at the breakpoint
 * @return True to continue execution, false to stay paused
 */
using BreakpointCallback = std::function<bool(class IScriptEngine*, const ScriptDebugInfo&)>;

/**
 * @brief Interface for all script engines
 * 
 * This abstract class defines the common functionality that all
 * script engines must implement, regardless of the underlying 
 * scripting language or implementation.
 */
class IScriptEngine {
public:
    virtual ~IScriptEngine() = default;

    /**
     * @brief Initialize the script engine
     * @param gameSession Pointer to the current game session
     * @return True if initialization succeeds
     */
    virtual bool initialize(GameSession* gameSession) = 0;

    /**
     * @brief Shutdown the script engine
     * @return True if shutdown succeeds
     */
    virtual bool shutdown() = 0;

    /**
     * @brief Get the script language supported by this engine
     * @return The script language
     */
    virtual ScriptLanguage getLanguage() const = 0;

    /**
     * @brief Get the script language name as a string
     * @return The script language name
     */
    virtual std::string getLanguageName() const = 0;

    /**
     * @brief Execute a script from string
     * @param scriptCode The script code as a string
     * @param scriptName Optional name for the script (for debugging)
     * @return Result of the script execution
     */
    virtual ScriptResult executeString(const std::string& scriptCode, 
                                      const std::string& scriptName = "unnamed") = 0;

    /**
     * @brief Execute a script from file
     * @param filename Path to the script file
     * @return Result of the script execution
     */
    virtual ScriptResult executeFile(const std::string& filename) = 0;

    /**
     * @brief Call a specific function in a loaded script
     * @param functionName Name of the function to call
     * @param args Arguments to pass to the function
     * @return Result of the function call
     */
    virtual ScriptResult callFunction(const std::string& functionName, 
                                     const std::vector<std::any>& args = {}) = 0;

    /**
     * @brief Register a C++ function to be callable from script
     * @param name Name of the function in scripts
     * @param function C++ function to bind
     * @return True if registration succeeds
     */
    virtual bool registerFunction(const std::string& name, 
                                 std::function<std::any(const std::vector<std::any>&)> function) = 0;

    /**
     * @brief Register a C++ class to be usable from script
     * @tparam T Type of the class to register
     * @param className Name of the class in scripts
     * @return True if registration succeeds
     */
    template<typename T>
    bool registerClass(const std::string& className) {
        return registerClassInternal(className, typeid(T));
    }

    /**
     * @brief Set a global variable in the script environment
     * @param name Name of the variable
     * @param value Value to set
     * @return True if setting succeeds
     */
    virtual bool setGlobal(const std::string& name, const std::any& value) = 0;

    /**
     * @brief Get a global variable from the script environment
     * @param name Name of the variable
     * @return Value of the variable if it exists
     */
    virtual std::optional<std::any> getGlobal(const std::string& name) = 0;

    /**
     * @brief Check if a global variable exists
     * @param name Name of the variable
     * @return True if the variable exists
     */
    virtual bool hasGlobal(const std::string& name) = 0;

    /**
     * @brief Add a breakpoint to a script
     * @param breakpoint Breakpoint to add
     * @return True if breakpoint was set successfully
     */
    virtual bool addBreakpoint(const Breakpoint& breakpoint) = 0;

    /**
     * @brief Remove a breakpoint
     * @param scriptName Script file name
     * @param lineNumber Line number of the breakpoint
     * @return True if breakpoint was removed
     */
    virtual bool removeBreakpoint(const std::string& scriptName, int lineNumber) = 0;

    /**
     * @brief Enable or disable a breakpoint
     * @param scriptName Script file name
     * @param lineNumber Line number of the breakpoint
     * @param enabled Whether the breakpoint should be enabled
     * @return True if breakpoint state was changed
     */
    virtual bool enableBreakpoint(const std::string& scriptName, int lineNumber, bool enabled) = 0;

    /**
     * @brief Set breakpoint callback
     * @param callback Function to call when a breakpoint is hit
     */
    virtual void setBreakpointCallback(BreakpointCallback callback) = 0;

    /**
     * @brief Continue execution after a breakpoint
     * @return True if execution continues
     */
    virtual bool continueExecution() = 0;

    /**
     * @brief Step to the next line during debugging
     * @return True if step succeeds
     */
    virtual bool stepOver() = 0;

    /**
     * @brief Step into a function during debugging
     * @return True if step succeeds
     */
    virtual bool stepInto() = 0;

    /**
     * @brief Step out of current function during debugging
     * @return True if step succeeds
     */
    virtual bool stepOut() = 0;

    /**
     * @brief Get current debug information
     * @return Current debug information if available
     */
    virtual std::optional<ScriptDebugInfo> getDebugInfo() = 0;

    /**
     * @brief Set sandbox mode for script execution
     * @param enabled Whether sandbox mode should be enabled
     * @return True if mode was set successfully
     */
    virtual bool setSandboxMode(bool enabled) = 0;

    /**
     * @brief Check if sandbox mode is enabled
     * @return True if sandbox mode is enabled
     */
    virtual bool isSandboxModeEnabled() const = 0;

    /**
     * @brief Hot-reload a script file
     * @param filename Path to the script file
     * @return True if reload succeeds
     */
    virtual bool hotReloadFile(const std::string& filename) = 0;

    /**
     * @brief Enable profiling of script execution
     * @param enabled Whether profiling should be enabled
     */
    virtual void enableProfiling(bool enabled) = 0;

    /**
     * @brief Get profiling results
     * @return Map of function names to execution times in milliseconds
     */
    virtual std::unordered_map<std::string, double> getProfilingResults() const = 0;

    /**
     * @brief Reset profiling data
     */
    virtual void resetProfilingData() = 0;

    /**
     * @brief Validate a script without executing it
     * @param scriptCode The script code as a string
     * @param scriptName Optional name for the script
     * @return Result with success=true if validation passes
     */
    virtual ScriptResult validateScript(const std::string& scriptCode, 
                                       const std::string& scriptName = "unnamed") = 0;

    /**
     * @brief Set error handler for script exceptions
     * @param handler Function to call when a script error occurs
     */
    virtual void setErrorHandler(std::function<void(const std::string&, int, const std::string&)> handler) = 0;

    /**
     * @brief Expose an entity to the script environment
     * @param entity Entity to expose
     * @param name Name to use in scripts
     * @return True if entity was exposed successfully
     */
    virtual bool exposeEntity(Entity* entity, const std::string& name) = 0;

    /**
     * @brief Create a script sandbox with limited permissions
     * @param name Name of the sandbox
     * @return True if sandbox was created successfully
     */
    virtual bool createSandbox(const std::string& name) = 0;

    /**
     * @brief Execute a script in a specific sandbox
     * @param sandboxName Name of the sandbox
     * @param scriptCode The script code as a string
     * @param scriptName Optional name for the script
     * @return Result of the script execution
     */
    virtual ScriptResult executeInSandbox(const std::string& sandboxName, 
                                         const std::string& scriptCode,
                                         const std::string& scriptName = "unnamed") = 0;

    /**
     * @brief Get the version of the scripting engine
     * @return Version string
     */
    virtual std::string getVersion() const = 0;

protected:
    /**
     * @brief Internal implementation of class registration
     * @param className Name of the class in scripts
     * @param typeIndex Type information for the class
     * @return True if registration succeeds
     */
    virtual bool registerClassInternal(const std::string& className, 
                                      const std::type_index& typeIndex) = 0;
};

/**
 * @brief Get script language from string name
 * @param name Language name (e.g., "lua", "javascript", "python")
 * @return Script language enum value
 */
inline ScriptLanguage scriptLanguageFromString(const std::string& name) {
    if (name == "lua" || name == "Lua") return ScriptLanguage::Lua;
    if (name == "javascript" || name == "JavaScript" || name == "js") return ScriptLanguage::JavaScript;
    if (name == "python" || name == "Python" || name == "py") return ScriptLanguage::Python;
    return ScriptLanguage::Custom;
}

/**
 * @brief Convert script language enum to string
 * @param language Script language enum value
 * @return Language name string
 */
inline std::string scriptLanguageToString(ScriptLanguage language) {
    switch (language) {
        case ScriptLanguage::Lua: return "Lua";
        case ScriptLanguage::JavaScript: return "JavaScript";
        case ScriptLanguage::Python: return "Python";
        case ScriptLanguage::Custom: return "Custom";
        default: return "Unknown";
    }
}

} // namespace Scripting