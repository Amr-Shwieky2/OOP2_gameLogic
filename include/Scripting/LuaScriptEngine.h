#pragma once

#include "Scripting/IScriptEngine.h"
#include <string>
#include <memory>
#include <vector>
#include <any>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <typeindex>
#include <mutex>
#include <chrono>

#if defined(USE_LUA_STUB)
// Use our stub version
#include "lua_stub.h"
struct lua_State { int dummy; };
struct luaL_Reg { const char* name; void* func; };
#else
// Forward declarations for Lua
struct lua_State;
struct luaL_Reg;
#endif

namespace Scripting {

/**
 * @brief Lua script engine implementation
 * 
 * This class provides a Lua-based implementation of the IScriptEngine interface.
 * It handles loading and executing Lua scripts, binding C++ functions and classes
 * to Lua, and providing debugging support.
 */
class LuaScriptEngine : public IScriptEngine {
public:
    /**
     * @brief Constructor
     */
    LuaScriptEngine();
    
    /**
     * @brief Destructor
     */
    ~LuaScriptEngine() override;

    // IScriptEngine interface implementation
    bool initialize(GameSession* gameSession) override;
    bool shutdown() override;
    ScriptLanguage getLanguage() const override { return ScriptLanguage::Lua; }
    std::string getLanguageName() const override { return "Lua"; }
    ScriptResult executeString(const std::string& scriptCode, const std::string& scriptName = "unnamed") override;
    ScriptResult executeFile(const std::string& filename) override;
    ScriptResult callFunction(const std::string& functionName, const std::vector<std::any>& args = {}) override;
    bool registerFunction(const std::string& name, std::function<std::any(const std::vector<std::any>&)> function) override;
    bool setGlobal(const std::string& name, const std::any& value) override;
    std::optional<std::any> getGlobal(const std::string& name) override;
    bool hasGlobal(const std::string& name) override;
    bool addBreakpoint(const Breakpoint& breakpoint) override;
    bool removeBreakpoint(const std::string& scriptName, int lineNumber) override;
    bool enableBreakpoint(const std::string& scriptName, int lineNumber, bool enabled) override;
    void setBreakpointCallback(BreakpointCallback callback) override;
    bool continueExecution() override;
    bool stepOver() override;
    bool stepInto() override;
    bool stepOut() override;
    std::optional<ScriptDebugInfo> getDebugInfo() override;
    bool setSandboxMode(bool enabled) override;
    bool isSandboxModeEnabled() const override { return m_sandboxEnabled; }
    bool hotReloadFile(const std::string& filename) override;
    void enableProfiling(bool enabled) override;
    std::unordered_map<std::string, double> getProfilingResults() const override;
    void resetProfilingData() override;
    ScriptResult validateScript(const std::string& scriptCode, const std::string& scriptName = "unnamed") override;
    void setErrorHandler(std::function<void(const std::string&, int, const std::string&)> handler) override;
    bool exposeEntity(Entity* entity, const std::string& name) override;
    bool createSandbox(const std::string& name) override;
    ScriptResult executeInSandbox(const std::string& sandboxName, const std::string& scriptCode, const std::string& scriptName = "unnamed") override;
    std::string getVersion() const override;

protected:
    bool registerClassInternal(const std::string& className, const std::type_index& typeIndex) override;

private:
    /**
     * @brief Structure to hold Lua hook information
     */
    struct LuaHookInfo {
        bool active = false;            ///< Whether the hook is active
        int currentLine = 0;            ///< Current line number
        std::string currentFunction;    ///< Current function name
        std::string currentSource;      ///< Current source file
    };

    /**
     * @brief Structure to hold breakpoint information
     */
    struct LuaBreakpoint {
        Breakpoint info;                ///< Breakpoint information
        bool active = false;            ///< Whether the breakpoint was hit
    };

    /**
     * @brief Structure to hold sandbox information
     */
    struct LuaSandbox {
        lua_State* state = nullptr;     ///< Sandbox Lua state
        bool initialized = false;       ///< Whether the sandbox is initialized
    };

    // Helper methods
    void setupLuaState();
    void setupSandboxState(lua_State* L);
    void registerStandardLibraries(lua_State* L, bool restricted = false);
    void registerGameAPI(lua_State* L);
    void registerEntityAPI(lua_State* L);
    void registerComponentAPI(lua_State* L);
    void registerMathAPI(lua_State* L);
    void registerInputAPI(lua_State* L);
    void registerPhysicsAPI(lua_State* L);
    void registerAudioAPI(lua_State* L);
    void registerGraphicsAPI(lua_State* L);
    void registerUtilityAPI(lua_State* L);
    void setupDebugHook();
    
    // Conversion helpers between Lua and C++
    bool pushValueToLua(lua_State* L, const std::any& value);
    std::any getValueFromLua(lua_State* L, int index);
    ScriptResult handleLuaResult(int result, const std::string& context);
    std::string getLastError();
    
    // Debug helpers
    static void luaHookFunction(lua_State* L, lua_Debug* ar);
    ScriptDebugInfo buildDebugInfo();
    static int luaPanicHandler(lua_State* L);
    
    // Profiling helpers
    void beginProfiling(const std::string& functionName);
    void endProfiling(const std::string& functionName);
    
    // Sandboxing helpers
    lua_State* getSandboxState(const std::string& name);
    static int restrictedLoadLib(lua_State* L);
    
    // Class registration helpers
    template<typename T>
    void registerClassMethods(lua_State* L, const std::string& className);
    
    // Data members
    lua_State* m_luaState = nullptr;                          ///< Main Lua state
    GameSession* m_gameSession = nullptr;                     ///< Game session reference
    std::unordered_map<std::string, LuaSandbox> m_sandboxes;  ///< Script sandboxes
    
    // Debugging state
    LuaHookInfo m_hookInfo;                                   ///< Current hook information
    std::vector<LuaBreakpoint> m_breakpoints;                 ///< Registered breakpoints
    BreakpointCallback m_breakpointCallback;                  ///< Breakpoint callback function
    bool m_debugging = false;                                 ///< Whether we're in debugging mode
    bool m_stepOverMode = false;                              ///< Whether we're stepping over
    bool m_stepIntoMode = false;                              ///< Whether we're stepping into
    bool m_stepOutMode = false;                               ///< Whether we're stepping out
    int m_stepDepth = 0;                                      ///< Call depth for stepping
    bool m_continueExecution = false;                         ///< Whether to continue execution
    
    // Profiling state
    bool m_profilingEnabled = false;                          ///< Whether profiling is enabled
    std::unordered_map<std::string, double> m_profilingData;  ///< Profiling data
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> m_profilingStartTimes; ///< Start times for functions
    
    // Security state
    bool m_sandboxEnabled = false;                            ///< Whether sandbox mode is enabled
    
    // Error handling
    std::function<void(const std::string&, int, const std::string&)> m_errorHandler; ///< Error handler function
    
    // Thread safety
    mutable std::mutex m_mutex;                               ///< Mutex for thread safety
    
    // Type registry
    std::unordered_map<std::type_index, std::string> m_typeRegistry; ///< Mapping from C++ types to Lua class names
    
    // Function registry
    std::unordered_map<std::string, std::function<std::any(const std::vector<std::any>&)>> m_functionRegistry; ///< Registered C++ functions
    
    // Loaded scripts
    std::unordered_map<std::string, std::string> m_loadedScripts; ///< Map of script names to their sources
    
    // Entity registry
    std::unordered_map<std::string, Entity*> m_exposedEntities; ///< Entities exposed to Lua
    
    // Static Lua callback handlers
    static int luaFunctionDispatcher(lua_State* L);
    static int luaErrorHandler(lua_State* L);
};

} // namespace Scripting