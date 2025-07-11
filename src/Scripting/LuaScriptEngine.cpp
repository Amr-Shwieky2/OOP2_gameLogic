#include "Scripting/LuaScriptEngine.h"
#include "Entity.h"
#include "Component.h"
#include "GameSession.h"

#ifdef USE_LUA_STUB
#include "lua_stub.h" // Use our stub implementation
#else
#include <lua.hpp>    // Use real Lua
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <filesystem>

namespace Scripting {

// Static map to store Lua state to engine instance mapping for callbacks
static std::unordered_map<lua_State*, LuaScriptEngine*> s_luaStateToEngine;

// Helper to get engine from Lua state
static LuaScriptEngine* getEngineFromState(lua_State* L) {
    auto it = s_luaStateToEngine.find(L);
    if (it != s_luaStateToEngine.end()) {
        return it->second;
    }
    return nullptr;
}

// Debug hook function
void LuaScriptEngine::luaHookFunction(lua_State* L, lua_Debug* ar) {
    LuaScriptEngine* engine = getEngineFromState(L);
    if (!engine) return;

    // Get current line info
    lua_getinfo(L, "Sl", ar);
    engine->m_hookInfo.currentLine = ar->currentline;
    engine->m_hookInfo.currentSource = ar->short_src;

    // Get function info
    lua_getinfo(L, "n", ar);
    engine->m_hookInfo.currentFunction = ar->name ? ar->name : "?";

    // Check for breakpoints
    bool hitBreakpoint = false;
    for (auto& bp : engine->m_breakpoints) {
        if (bp.info.scriptName == engine->m_hookInfo.currentSource && 
            bp.info.lineNumber == engine->m_hookInfo.currentLine && 
            bp.info.enabled) {
            
            // Evaluate condition if present
            if (!bp.info.condition.empty()) {
                // Push the condition to the stack
                if (luaL_loadstring(L, bp.info.condition.c_str()) != 0 || 
                    lua_pcall(L, 0, 1, 0) != 0) {
                    // Condition failed to evaluate, ignore breakpoint
                    continue;
                }
                
                // Check if condition is true
                if (!lua_toboolean(L, -1)) {
                    lua_pop(L, 1);
                    continue;
                }
                
                lua_pop(L, 1);
            }
            
            hitBreakpoint = true;
            bp.active = true;
            break;
        }
    }

    // If we hit a breakpoint or are stepping, pause execution
    if (hitBreakpoint || engine->m_stepOverMode || engine->m_stepIntoMode || engine->m_stepOutMode) {
        // Build debug info
        ScriptDebugInfo debugInfo = engine->buildDebugInfo();

        // Call breakpoint callback if set
        if (engine->m_breakpointCallback) {
            bool shouldContinue = engine->m_breakpointCallback(engine, debugInfo);
            if (shouldContinue) {
                engine->m_continueExecution = true;
                return;
            }
        }

        // Pause execution by yielding
        engine->m_debugging = true;
        lua_yield(L, 0);
    }
}

// Lua panic handler
int LuaScriptEngine::luaPanicHandler(lua_State* L) {
    std::string error = lua_tostring(L, -1);
    std::cerr << "Lua panic: " << error << std::endl;
    
    // Get engine and call error handler if available
    LuaScriptEngine* engine = getEngineFromState(L);
    if (engine && engine->m_errorHandler) {
        engine->m_errorHandler("Script panic", -1, error);
    }
    
    return 0; // Return 0 to not trigger a longjmp
}

// C function to dispatch to C++ functions
int LuaScriptEngine::luaFunctionDispatcher(lua_State* L) {
    // Get function pointer from upvalue
    auto funcPtr = static_cast<std::function<std::any(const std::vector<std::any>&)>*>(
        lua_touserdata(L, lua_upvalueindex(1)));
    
    if (!funcPtr) {
        lua_pushstring(L, "Function dispatcher: Invalid function pointer");
        lua_error(L);
        return 0;
    }
    
    // Get engine
    LuaScriptEngine* engine = getEngineFromState(L);
    if (!engine) {
        lua_pushstring(L, "Function dispatcher: No engine associated with this Lua state");
        lua_error(L);
        return 0;
    }
    
    try {
        // Collect arguments
        int nargs = lua_gettop(L);
        std::vector<std::any> args;
        args.reserve(nargs);
        
        for (int i = 1; i <= nargs; ++i) {
            args.push_back(engine->getValueFromLua(L, i));
        }
        
        // Call the C++ function
        std::any result = (*funcPtr)(args);
        
        // Push result to Lua
        engine->pushValueToLua(L, result);
        
        return 1;
    }
    catch (const std::exception& e) {
        lua_pushstring(L, e.what());
        lua_error(L);
        return 0;
    }
    catch (...) {
        lua_pushstring(L, "Unknown error in function dispatcher");
        lua_error(L);
        return 0;
    }
}

// Error handler function
int LuaScriptEngine::luaErrorHandler(lua_State* L) {
    std::string errorMsg = lua_tostring(L, -1);
    
    // Get debug info
    lua_Debug ar;
    if (lua_getstack(L, 1, &ar)) {
        lua_getinfo(L, "Sl", &ar);
        
        std::ostringstream errorInfo;
        errorInfo << errorMsg << " [" << ar.short_src << ":" << ar.currentline << "]";
        
        // Replace the error message with the enhanced one
        lua_pop(L, 1);
        lua_pushstring(L, errorInfo.str().c_str());
    }
    
    return 1;
}

// Constructor
LuaScriptEngine::LuaScriptEngine()
    : m_luaState(nullptr)
    , m_gameSession(nullptr)
    , m_debugging(false)
    , m_stepOverMode(false)
    , m_stepIntoMode(false)
    , m_stepOutMode(false)
    , m_stepDepth(0)
    , m_continueExecution(false)
    , m_profilingEnabled(false)
    , m_sandboxEnabled(false)
{
}

// Destructor
LuaScriptEngine::~LuaScriptEngine() {
    shutdown();
}

// Initialize the script engine
bool LuaScriptEngine::initialize(GameSession* gameSession) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_gameSession = gameSession;
    
    // Create the Lua state
    m_luaState = luaL_newstate();
    if (!m_luaState) {
        std::cerr << "Failed to create Lua state" << std::endl;
        return false;
    }
    
    // Register this engine with the state
    s_luaStateToEngine[m_luaState] = this;
    
    // Setup the Lua state
    setupLuaState();
    
    return true;
}

// Shutdown the script engine
bool LuaScriptEngine::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Clean up sandboxes
    for (auto& [name, sandbox] : m_sandboxes) {
        if (sandbox.state) {
            lua_close(sandbox.state);
            sandbox.state = nullptr;
            sandbox.initialized = false;
        }
    }
    m_sandboxes.clear();
    
    // Close the main Lua state
    if (m_luaState) {
        // Remove from static map
        s_luaStateToEngine.erase(m_luaState);
        
        lua_close(m_luaState);
        m_luaState = nullptr;
    }
    
    m_gameSession = nullptr;
    m_loadedScripts.clear();
    m_exposedEntities.clear();
    
    return true;
}

// Set up the Lua state with standard libraries and game API
void LuaScriptEngine::setupLuaState() {
    if (!m_luaState) return;
    
    // Set panic handler
    lua_atpanic(m_luaState, luaPanicHandler);
    
    // Open standard libraries
    registerStandardLibraries(m_luaState);
    
    // Register game API
    registerGameAPI(m_luaState);
    
    // Set up debug hook
    setupDebugHook();
}

// Set up a sandbox state
void LuaScriptEngine::setupSandboxState(lua_State* L) {
    if (!L) return;
    
    // Set panic handler
    lua_atpanic(L, luaPanicHandler);
    
    // Register state with engine
    s_luaStateToEngine[L] = this;
    
    // Open restricted libraries
    registerStandardLibraries(L, true);
    
    // Register game API with restrictions
    // Note: In a real implementation, you'd want to provide restricted
    // versions of the API functions
    registerGameAPI(L);
}

// Register standard Lua libraries
void LuaScriptEngine::registerStandardLibraries(lua_State* L, bool restricted) {
    if (!L) return;
    
    if (restricted) {
        // Limited standard libraries for sandboxes
        luaL_requiref(L, "_G", luaopen_base, 1);
        lua_pop(L, 1);
        
        luaL_requiref(L, "string", luaopen_string, 1);
        lua_pop(L, 1);
        
        luaL_requiref(L, "table", luaopen_table, 1);
        lua_pop(L, 1);
        
        luaL_requiref(L, "math", luaopen_math, 1);
        lua_pop(L, 1);
        
        // Remove dangerous functions from base library
        lua_pushnil(L);
        lua_setglobal(L, "dofile");
        
        lua_pushnil(L);
        lua_setglobal(L, "loadfile");
        
        lua_pushnil(L);
        lua_setglobal(L, "load");
        
        lua_pushnil(L);
        lua_setglobal(L, "os");
        
        lua_pushnil(L);
        lua_setglobal(L, "io");
        
        // Add safe loader
        lua_pushcfunction(L, restrictedLoadLib);
        lua_setglobal(L, "require");
    }
    else {
        // Full standard libraries for main state
        luaL_openlibs(L);
    }
}

// Register game API functions
void LuaScriptEngine::registerGameAPI(lua_State* L) {
    if (!L) return;
    
    // Register entity API
    registerEntityAPI(L);
    
    // Register component API
    registerComponentAPI(L);
    
    // Register math API
    registerMathAPI(L);
    
    // Register input API
    registerInputAPI(L);
    
    // Register physics API
    registerPhysicsAPI(L);
    
    // Register audio API
    registerAudioAPI(L);
    
    // Register graphics API
    registerGraphicsAPI(L);
    
    // Register utility API
    registerUtilityAPI(L);
}

// Register entity API
void LuaScriptEngine::registerEntityAPI(lua_State* L) {
    if (!L) return;
    
    // Create entity library
    lua_newtable(L);
    
    // Sample entity API functions
    // In a real implementation, these would actually do something
    
    // entity.create(type)
    lua_pushstring(L, "create");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        const char* type = luaL_checkstring(L, 1);
        lua_pushinteger(L, 0); // Return dummy entity ID
        return 1;
    });
    lua_settable(L, -3);
    
    // entity.destroy(id)
    lua_pushstring(L, "destroy");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer id = luaL_checkinteger(L, 1);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // entity.getPosition(id)
    lua_pushstring(L, "getPosition");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer id = luaL_checkinteger(L, 1);
        lua_pushnumber(L, 0.0); // x
        lua_pushnumber(L, 0.0); // y
        return 2;
    });
    lua_settable(L, -3);
    
    // entity.setPosition(id, x, y)
    lua_pushstring(L, "setPosition");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer id = luaL_checkinteger(L, 1);
        lua_Number x = luaL_checknumber(L, 2);
        lua_Number y = luaL_checknumber(L, 3);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // Set the table as global
    lua_setglobal(L, "entity");
}

// Register component API
void LuaScriptEngine::registerComponentAPI(lua_State* L) {
    if (!L) return;
    
    // Create component library
    lua_newtable(L);
    
    // Sample component API functions
    // In a real implementation, these would actually do something
    
    // component.add(entityId, componentType)
    lua_pushstring(L, "add");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer entityId = luaL_checkinteger(L, 1);
        const char* type = luaL_checkstring(L, 2);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // component.remove(entityId, componentType)
    lua_pushstring(L, "remove");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer entityId = luaL_checkinteger(L, 1);
        const char* type = luaL_checkstring(L, 2);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // component.has(entityId, componentType)
    lua_pushstring(L, "has");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer entityId = luaL_checkinteger(L, 1);
        const char* type = luaL_checkstring(L, 2);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // Set the table as global
    lua_setglobal(L, "component");
}

// Register math API
void LuaScriptEngine::registerMathAPI(lua_State* L) {
    // Lua already has a good math library, just extend it if needed
}

// Register input API
void LuaScriptEngine::registerInputAPI(lua_State* L) {
    if (!L) return;
    
    // Create input library
    lua_newtable(L);
    
    // Sample input API functions
    // In a real implementation, these would actually do something
    
    // input.isKeyPressed(key)
    lua_pushstring(L, "isKeyPressed");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        const char* key = luaL_checkstring(L, 1);
        lua_pushboolean(L, 0);
        return 1;
    });
    lua_settable(L, -3);
    
    // input.isMouseButtonPressed(button)
    lua_pushstring(L, "isMouseButtonPressed");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer button = luaL_checkinteger(L, 1);
        lua_pushboolean(L, 0);
        return 1;
    });
    lua_settable(L, -3);
    
    // input.getMousePosition()
    lua_pushstring(L, "getMousePosition");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_pushnumber(L, 0.0); // x
        lua_pushnumber(L, 0.0); // y
        return 2;
    });
    lua_settable(L, -3);
    
    // Set the table as global
    lua_setglobal(L, "input");
}

// Register physics API
void LuaScriptEngine::registerPhysicsAPI(lua_State* L) {
    if (!L) return;
    
    // Create physics library
    lua_newtable(L);
    
    // Sample physics API functions
    // In a real implementation, these would actually do something
    
    // physics.applyForce(entityId, forceX, forceY)
    lua_pushstring(L, "applyForce");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer entityId = luaL_checkinteger(L, 1);
        lua_Number forceX = luaL_checknumber(L, 2);
        lua_Number forceY = luaL_checknumber(L, 3);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // physics.setVelocity(entityId, velocityX, velocityY)
    lua_pushstring(L, "setVelocity");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer entityId = luaL_checkinteger(L, 1);
        lua_Number velocityX = luaL_checknumber(L, 2);
        lua_Number velocityY = luaL_checknumber(L, 3);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // physics.getVelocity(entityId)
    lua_pushstring(L, "getVelocity");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer entityId = luaL_checkinteger(L, 1);
        lua_pushnumber(L, 0.0); // velocityX
        lua_pushnumber(L, 0.0); // velocityY
        return 2;
    });
    lua_settable(L, -3);
    
    // Set the table as global
    lua_setglobal(L, "physics");
}

// Register audio API
void LuaScriptEngine::registerAudioAPI(lua_State* L) {
    if (!L) return;
    
    // Create audio library
    lua_newtable(L);
    
    // Sample audio API functions
    // In a real implementation, these would actually do something
    
    // audio.playSound(name)
    lua_pushstring(L, "playSound");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        const char* name = luaL_checkstring(L, 1);
        lua_pushinteger(L, 0); // soundId
        return 1;
    });
    lua_settable(L, -3);
    
    // audio.stopSound(soundId)
    lua_pushstring(L, "stopSound");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer soundId = luaL_checkinteger(L, 1);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // audio.setVolume(volume)
    lua_pushstring(L, "setVolume");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Number volume = luaL_checknumber(L, 1);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // Set the table as global
    lua_setglobal(L, "audio");
}

// Register graphics API
void LuaScriptEngine::registerGraphicsAPI(lua_State* L) {
    if (!L) return;
    
    // Create graphics library
    lua_newtable(L);
    
    // Sample graphics API functions
    // In a real implementation, these would actually do something
    
    // graphics.setVisible(entityId, visible)
    lua_pushstring(L, "setVisible");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer entityId = luaL_checkinteger(L, 1);
        lua_Boolean visible = lua_toboolean(L, 2);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // graphics.setTexture(entityId, textureName)
    lua_pushstring(L, "setTexture");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer entityId = luaL_checkinteger(L, 1);
        const char* textureName = luaL_checkstring(L, 2);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // graphics.setColor(entityId, r, g, b, a)
    lua_pushstring(L, "setColor");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Integer entityId = luaL_checkinteger(L, 1);
        lua_Integer r = luaL_checkinteger(L, 2);
        lua_Integer g = luaL_checkinteger(L, 3);
        lua_Integer b = luaL_checkinteger(L, 4);
        lua_Integer a = luaL_optinteger(L, 5, 255);
        lua_pushboolean(L, 1);
        return 1;
    });
    lua_settable(L, -3);
    
    // Set the table as global
    lua_setglobal(L, "graphics");
}

// Register utility API
void LuaScriptEngine::registerUtilityAPI(lua_State* L) {
    if (!L) return;
    
    // Create utility library
    lua_newtable(L);
    
    // Sample utility API functions
    // In a real implementation, these would actually do something
    
    // utils.log(message)
    lua_pushstring(L, "log");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        const char* message = luaL_checkstring(L, 1);
        std::cout << "Script log: " << message << std::endl;
        return 0;
    });
    lua_settable(L, -3);
    
    // utils.random(min, max)
    lua_pushstring(L, "random");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_Number min = luaL_checknumber(L, 1);
        lua_Number max = luaL_checknumber(L, 2);
        lua_Number value = min + static_cast<lua_Number>(rand()) / (static_cast<lua_Number>(RAND_MAX / (max - min)));
        lua_pushnumber(L, value);
        return 1;
    });
    lua_settable(L, -3);
    
    // utils.getTime()
    lua_pushstring(L, "getTime");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        auto now = std::chrono::system_clock::now();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        lua_pushnumber(L, static_cast<lua_Number>(seconds));
        return 1;
    });
    lua_settable(L, -3);
    
    // Set the table as global
    lua_setglobal(L, "utils");
}

// Set up debug hook
void LuaScriptEngine::setupDebugHook() {
    if (!m_luaState) return;
    
    // Set debug hook
    lua_sethook(m_luaState, luaHookFunction, LUA_MASKLINE, 0);
}

// Execute a script from string
ScriptResult LuaScriptEngine::executeString(const std::string& scriptCode, const std::string& scriptName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_luaState) {
        ScriptResult result;
        result.success = false;
        result.error = "Lua state is not initialized";
        return result;
    }
    
    // Store script for hot-reloading
    m_loadedScripts[scriptName] = scriptCode;
    
    // Start profiling if enabled
    auto startTime = m_profilingEnabled ? 
        std::chrono::high_resolution_clock::now() : 
        std::chrono::high_resolution_clock::time_point();
    
    // Set up error handler
    lua_pushcfunction(m_luaState, luaErrorHandler);
    int errorHandlerIndex = lua_gettop(m_luaState);
    
    // Load the script
    if (luaL_loadbuffer(m_luaState, scriptCode.c_str(), scriptCode.size(), scriptName.c_str()) != 0) {
        std::string error = lua_tostring(m_luaState, -1);
        lua_pop(m_luaState, 2); // Error and error handler
        
        ScriptResult result;
        result.success = false;
        result.error = error;
        
        // Extract line number from error message
        size_t linePos = error.find(':', scriptName.length() + 1);
        if (linePos != std::string::npos) {
            size_t lineEnd = error.find(':', linePos + 1);
            if (lineEnd != std::string::npos) {
                try {
                    result.errorLine = std::stoi(error.substr(linePos + 1, lineEnd - linePos - 1));
                }
                catch (...) {
                    result.errorLine = -1;
                }
            }
        }
        
        return result;
    }
    
    // Execute the script
    int result = lua_pcall(m_luaState, 0, LUA_MULTRET, errorHandlerIndex);
    
    // End profiling if enabled
    if (m_profilingEnabled) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        m_profilingData[scriptName] = elapsed;
    }
    
    // Remove error handler
    lua_remove(m_luaState, errorHandlerIndex);
    
    // Build result
    return handleLuaResult(result, scriptName);
}

// Execute a script from file
ScriptResult LuaScriptEngine::executeFile(const std::string& filename) {
    // Check if file exists
    if (!std::filesystem::exists(filename)) {
        ScriptResult result;
        result.success = false;
        result.error = "File not found: " + filename;
        return result;
    }
    
    // Read the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        ScriptResult result;
        result.success = false;
        result.error = "Failed to open file: " + filename;
        return result;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    // Execute the script
    return executeString(buffer.str(), filename);
}

// Call a function in the script
ScriptResult LuaScriptEngine::callFunction(const std::string& functionName, const std::vector<std::any>& args) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_luaState) {
        ScriptResult result;
        result.success = false;
        result.error = "Lua state is not initialized";
        return result;
    }
    
    // Start profiling if enabled
    auto startTime = m_profilingEnabled ? 
        std::chrono::high_resolution_clock::now() : 
        std::chrono::high_resolution_clock::time_point();
    
    // Set up error handler
    lua_pushcfunction(m_luaState, luaErrorHandler);
    int errorHandlerIndex = lua_gettop(m_luaState);
    
    // Get the function
    lua_getglobal(m_luaState, functionName.c_str());
    
    if (!lua_isfunction(m_luaState, -1)) {
        lua_pop(m_luaState, 2); // Function and error handler
        
        ScriptResult result;
        result.success = false;
        result.error = "Function not found: " + functionName;
        return result;
    }
    
    // Push arguments
    for (const auto& arg : args) {
        pushValueToLua(m_luaState, arg);
    }
    
    // Call the function
    int result = lua_pcall(m_luaState, static_cast<int>(args.size()), 1, errorHandlerIndex);
    
    // End profiling if enabled
    if (m_profilingEnabled) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        m_profilingData[functionName] = elapsed;
    }
    
    // Build result
    ScriptResult scriptResult = handleLuaResult(result, functionName);
    
    // Get return value if successful
    if (scriptResult.success && lua_gettop(m_luaState) > errorHandlerIndex) {
        scriptResult.returnValue = getValueFromLua(m_luaState, -1);
        lua_pop(m_luaState, 1); // Pop return value
    }
    
    // Remove error handler
    lua_remove(m_luaState, errorHandlerIndex);
    
    return scriptResult;
}

// Register a C++ function with Lua
bool LuaScriptEngine::registerFunction(const std::string& name, 
                                      std::function<std::any(const std::vector<std::any>&)> function) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_luaState) {
        return false;
    }
    
    // Store the function in the registry
    m_functionRegistry[name] = function;
    
    // Create a userdata with the function pointer
    auto* funcPtr = static_cast<std::function<std::any(const std::vector<std::any>&)>*>(
        lua_newuserdata(m_luaState, sizeof(std::function<std::any(const std::vector<std::any>&)>)));
    
    new (funcPtr) std::function<std::any(const std::vector<std::any>&)>(function);
    
    // Create closure with the function pointer as upvalue
    lua_pushcclosure(m_luaState, luaFunctionDispatcher, 1);
    
    // Set as global
    lua_setglobal(m_luaState, name.c_str());
    
    return true;
}

// Set a global variable in Lua
bool LuaScriptEngine::setGlobal(const std::string& name, const std::any& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_luaState) {
        return false;
    }
    
    // Push the value
    if (!pushValueToLua(m_luaState, value)) {
        return false;
    }
    
    // Set as global
    lua_setglobal(m_luaState, name.c_str());
    
    return true;
}

// Get a global variable from Lua
std::optional<std::any> LuaScriptEngine::getGlobal(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_luaState) {
        return std::nullopt;
    }
    
    // Get the global
    lua_getglobal(m_luaState, name.c_str());
    
    // Check if it exists
    if (lua_isnil(m_luaState, -1)) {
        lua_pop(m_luaState, 1);
        return std::nullopt;
    }
    
    // Convert to C++ value
    std::any value = getValueFromLua(m_luaState, -1);
    lua_pop(m_luaState, 1);
    
    return value;
}

// Check if a global variable exists
bool LuaScriptEngine::hasGlobal(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_luaState) {
        return false;
    }
    
    // Get the global
    lua_getglobal(m_luaState, name.c_str());
    
    // Check if it exists
    bool exists = !lua_isnil(m_luaState, -1);
    lua_pop(m_luaState, 1);
    
    return exists;
}

// Add a breakpoint
bool LuaScriptEngine::addBreakpoint(const Breakpoint& breakpoint) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check for duplicate
    for (const auto& bp : m_breakpoints) {
        if (bp.info.scriptName == breakpoint.scriptName && bp.info.lineNumber == breakpoint.lineNumber) {
            return false; // Already exists
        }
    }
    
    // Add the breakpoint
    LuaBreakpoint bp;
    bp.info = breakpoint;
    bp.active = false;
    m_breakpoints.push_back(bp);
    
    return true;
}

// Remove a breakpoint
bool LuaScriptEngine::removeBreakpoint(const std::string& scriptName, int lineNumber) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find_if(m_breakpoints.begin(), m_breakpoints.end(),
        [&](const LuaBreakpoint& bp) {
            return bp.info.scriptName == scriptName && bp.info.lineNumber == lineNumber;
        });
    
    if (it != m_breakpoints.end()) {
        m_breakpoints.erase(it);
        return true;
    }
    
    return false;
}

// Enable or disable a breakpoint
bool LuaScriptEngine::enableBreakpoint(const std::string& scriptName, int lineNumber, bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find_if(m_breakpoints.begin(), m_breakpoints.end(),
        [&](const LuaBreakpoint& bp) {
            return bp.info.scriptName == scriptName && bp.info.lineNumber == lineNumber;
        });
    
    if (it != m_breakpoints.end()) {
        it->info.enabled = enabled;
        return true;
    }
    
    return false;
}

// Set breakpoint callback
void LuaScriptEngine::setBreakpointCallback(BreakpointCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_breakpointCallback = callback;
}

// Continue execution after a breakpoint
bool LuaScriptEngine::continueExecution() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_debugging) {
        return false;
    }
    
    // Reset step modes
    m_stepOverMode = false;
    m_stepIntoMode = false;
    m_stepOutMode = false;
    
    // Allow execution to continue
    m_continueExecution = true;
    m_debugging = false;
    
    return true;
}

// Step over to the next line
bool LuaScriptEngine::stepOver() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_debugging) {
        return false;
    }
    
    m_stepOverMode = true;
    m_stepIntoMode = false;
    m_stepOutMode = false;
    m_stepDepth = 0;
    m_continueExecution = true;
    
    return true;
}

// Step into a function
bool LuaScriptEngine::stepInto() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_debugging) {
        return false;
    }
    
    m_stepOverMode = false;
    m_stepIntoMode = true;
    m_stepOutMode = false;
    m_continueExecution = true;
    
    return true;
}

// Step out of the current function
bool LuaScriptEngine::stepOut() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_debugging) {
        return false;
    }
    
    m_stepOverMode = false;
    m_stepIntoMode = false;
    m_stepOutMode = true;
    m_continueExecution = true;
    
    return true;
}

// Get current debug info
std::optional<ScriptDebugInfo> LuaScriptEngine::getDebugInfo() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_debugging || !m_luaState) {
        return std::nullopt;
    }
    
    return buildDebugInfo();
}

// Build debug info from current state
ScriptDebugInfo LuaScriptEngine::buildDebugInfo() {
    ScriptDebugInfo info;
    
    if (!m_luaState) {
        return info;
    }
    
    // Set basic info
    info.scriptName = m_hookInfo.currentSource;
    info.lineNumber = m_hookInfo.currentLine;
    info.functionName = m_hookInfo.currentFunction;
    
    // Build stack trace
    lua_Debug ar;
    int level = 0;
    std::ostringstream stackTrace;
    
    while (lua_getstack(m_luaState, level, &ar)) {
        lua_getinfo(m_luaState, "Snl", &ar);
        
        stackTrace << level << ": " << (ar.name ? ar.name : "?");
        stackTrace << " [" << ar.short_src << ":" << ar.currentline << "]";
        
        if (level > 0) {
            stackTrace << "\n";
        }
        
        // Get local variables at this level
        if (level == 0) { // Only for the top frame
            for (int i = 1; ; i++) {
                const char* name = lua_getlocal(m_luaState, &ar, i);
                if (!name) break;
                
                // Skip internal variables
                if (name[0] != '(') {
                    info.localVariables[name] = getValueFromLua(m_luaState, -1);
                }
                
                lua_pop(m_luaState, 1);
            }
        }
        
        level++;
    }
    
    info.stackTrace = stackTrace.str();
    
    return info;
}

// Set sandbox mode
bool LuaScriptEngine::setSandboxMode(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sandboxEnabled = enabled;
    return true;
}

// Hot-reload a script file
bool LuaScriptEngine::hotReloadFile(const std::string& filename) {
    // Simply re-execute the file
    return executeFile(filename).success;
}

// Enable profiling
void LuaScriptEngine::enableProfiling(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_profilingEnabled = enabled;
    
    if (!enabled) {
        resetProfilingData();
    }
}

// Get profiling results
std::unordered_map<std::string, double> LuaScriptEngine::getProfilingResults() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_profilingData;
}

// Reset profiling data
void LuaScriptEngine::resetProfilingData() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_profilingData.clear();
}

// Validate a script without executing it
ScriptResult LuaScriptEngine::validateScript(const std::string& scriptCode, const std::string& scriptName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_luaState) {
        ScriptResult result;
        result.success = false;
        result.error = "Lua state is not initialized";
        return result;
    }
    
    // Load the script but don't execute it
    int result = luaL_loadbuffer(m_luaState, scriptCode.c_str(), scriptCode.size(), scriptName.c_str());
    
    // Build result
    ScriptResult scriptResult;
    
    if (result != LUA_OK) {
        std::string error = lua_tostring(m_luaState, -1);
        lua_pop(m_luaState, 1);
        
        scriptResult.success = false;
        scriptResult.error = error;
        
        // Extract line number from error message
        size_t linePos = error.find(':', scriptName.length() + 1);
        if (linePos != std::string::npos) {
            size_t lineEnd = error.find(':', linePos + 1);
            if (lineEnd != std::string::npos) {
                try {
                    scriptResult.errorLine = std::stoi(error.substr(linePos + 1, lineEnd - linePos - 1));
                }
                catch (...) {
                    scriptResult.errorLine = -1;
                }
            }
        }
    }
    else {
        // Script is valid, pop the compiled function
        lua_pop(m_luaState, 1);
        scriptResult.success = true;
    }
    
    return scriptResult;
}

// Set error handler
void LuaScriptEngine::setErrorHandler(std::function<void(const std::string&, int, const std::string&)> handler) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_errorHandler = handler;
}

// Expose an entity to Lua
bool LuaScriptEngine::exposeEntity(Entity* entity, const std::string& name) {
    if (!entity || !m_luaState) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Store the entity
    m_exposedEntities[name] = entity;
    
    // Create userdata for the entity
    Entity** udata = static_cast<Entity**>(lua_newuserdata(m_luaState, sizeof(Entity*)));
    *udata = entity;
    
    // Set metatable for the userdata
    luaL_newmetatable(m_luaState, "EntityMetatable");
    
    // __index metamethod
    lua_pushstring(m_luaState, "__index");
    lua_pushcfunction(m_luaState, [](lua_State* L) -> int {
        Entity* entity = *static_cast<Entity**>(luaL_checkudata(L, 1, "EntityMetatable"));
        const char* key = luaL_checkstring(L, 2);
        
        // Check for special methods
        if (strcmp(key, "getId") == 0) {
            lua_pushinteger(L, entity->getId());
            return 1;
        }
        else if (strcmp(key, "isActive") == 0) {
            lua_pushboolean(L, entity->isActive());
            return 1;
        }
        else if (strcmp(key, "setActive") == 0) {
            lua_pushcfunction(L, [](lua_State* L) -> int {
                Entity* entity = *static_cast<Entity**>(luaL_checkudata(L, 1, "EntityMetatable"));
                bool active = lua_toboolean(L, 2);
                entity->setActive(active);
                return 0;
            });
            return 1;
        }
        else if (strcmp(key, "getComponent") == 0) {
            lua_pushcfunction(L, [](lua_State* L) -> int {
                Entity* entity = *static_cast<Entity**>(luaL_checkudata(L, 1, "EntityMetatable"));
                const char* componentName = luaL_checkstring(L, 2);
                
                // Get the component
                Component* component = entity->operator[](componentName);
                
                if (!component) {
                    lua_pushnil(L);
                    return 1;
                }
                
                // Create userdata for the component
                Component** udata = static_cast<Component**>(lua_newuserdata(L, sizeof(Component*)));
                *udata = component;
                
                // Set metatable for the component
                luaL_newmetatable(L, "ComponentMetatable");
                lua_setmetatable(L, -2);
                
                return 1;
            });
            return 1;
        }
        
        // Try to get a component
        Component* component = entity->operator[](key);
        if (component) {
            Component** udata = static_cast<Component**>(lua_newuserdata(L, sizeof(Component*)));
            *udata = component;
            luaL_newmetatable(L, "ComponentMetatable");
            lua_setmetatable(L, -2);
            return 1;
        }
        
        lua_pushnil(L);
        return 1;
    });
    lua_settable(m_luaState, -3);
    
    // Set the metatable
    lua_setmetatable(m_luaState, -2);
    
    // Set as global
    lua_setglobal(m_luaState, name.c_str());
    
    return true;
}

// Create a sandbox
bool LuaScriptEngine::createSandbox(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if sandbox already exists
    auto it = m_sandboxes.find(name);
    if (it != m_sandboxes.end() && it->second.initialized) {
        return true;
    }
    
    // Create a new Lua state for the sandbox
    lua_State* L = luaL_newstate();
    if (!L) {
        return false;
    }
    
    // Set up the sandbox state
    setupSandboxState(L);
    
    // Store the sandbox
    LuaSandbox sandbox;
    sandbox.state = L;
    sandbox.initialized = true;
    m_sandboxes[name] = sandbox;
    
    return true;
}

// Execute a script in a sandbox
ScriptResult LuaScriptEngine::executeInSandbox(const std::string& sandboxName, 
                                             const std::string& scriptCode,
                                             const std::string& scriptName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Get the sandbox
    lua_State* L = getSandboxState(sandboxName);
    if (!L) {
        ScriptResult result;
        result.success = false;
        result.error = "Sandbox not found: " + sandboxName;
        return result;
    }
    
    // Start profiling if enabled
    auto startTime = m_profilingEnabled ? 
        std::chrono::high_resolution_clock::now() : 
        std::chrono::high_resolution_clock::time_point();
    
    // Set up error handler
    lua_pushcfunction(L, luaErrorHandler);
    int errorHandlerIndex = lua_gettop(L);
    
    // Load the script
    if (luaL_loadbuffer(L, scriptCode.c_str(), scriptCode.size(), scriptName.c_str()) != 0) {
        std::string error = lua_tostring(L, -1);
        lua_pop(L, 2); // Error and error handler
        
        ScriptResult result;
        result.success = false;
        result.error = error;
        
        // Extract line number from error message
        size_t linePos = error.find(':', scriptName.length() + 1);
        if (linePos != std::string::npos) {
            size_t lineEnd = error.find(':', linePos + 1);
            if (lineEnd != std::string::npos) {
                try {
                    result.errorLine = std::stoi(error.substr(linePos + 1, lineEnd - linePos - 1));
                }
                catch (...) {
                    result.errorLine = -1;
                }
            }
        }
        
        return result;
    }
    
    // Execute the script
    int result = lua_pcall(L, 0, LUA_MULTRET, errorHandlerIndex);
    
    // End profiling if enabled
    if (m_profilingEnabled) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        m_profilingData[scriptName + " (sandbox: " + sandboxName + ")"] = elapsed;
    }
    
    // Remove error handler
    lua_remove(L, errorHandlerIndex);
    
    // Build result
    ScriptResult scriptResult;
    
    if (result != LUA_OK) {
        scriptResult.success = false;
        scriptResult.error = lua_tostring(L, -1);
        lua_pop(L, 1);
        
        // Extract line number from error message
        size_t linePos = scriptResult.error.find(':', scriptName.length() + 1);
        if (linePos != std::string::npos) {
            size_t lineEnd = scriptResult.error.find(':', linePos + 1);
            if (lineEnd != std::string::npos) {
                try {
                    scriptResult.errorLine = std::stoi(scriptResult.error.substr(linePos + 1, lineEnd - linePos - 1));
                }
                catch (...) {
                    scriptResult.errorLine = -1;
                }
            }
        }
        
        // Call error handler if set
        if (m_errorHandler) {
            m_errorHandler(scriptName, scriptResult.errorLine, scriptResult.error);
        }
    }
    else {
        scriptResult.success = true;
    }
    
    scriptResult.executionTimeMs = 0.0; // Not tracked for now
    
    return scriptResult;
}

// Get the Lua version
std::string LuaScriptEngine::getVersion() const {
    return LUA_VERSION;
}

// Register a class type with Lua
bool LuaScriptEngine::registerClassInternal(const std::string& className, const std::type_index& typeIndex) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_luaState) {
        return false;
    }
    
    // Store the type info
    m_typeRegistry[typeIndex] = className;
    
    return true;
}

// Push a C++ value to Lua
bool LuaScriptEngine::pushValueToLua(lua_State* L, const std::any& value) {
    if (!L) {
        return false;
    }
    
    try {
        if (!value.has_value()) {
            lua_pushnil(L);
            return true;
        }
        
        const std::type_info& type = value.type();
        
        if (type == typeid(bool)) {
            lua_pushboolean(L, std::any_cast<bool>(value));
        }
        else if (type == typeid(int)) {
            lua_pushinteger(L, std::any_cast<int>(value));
        }
        else if (type == typeid(unsigned int)) {
            lua_pushinteger(L, static_cast<lua_Integer>(std::any_cast<unsigned int>(value)));
        }
        else if (type == typeid(long)) {
            lua_pushinteger(L, static_cast<lua_Integer>(std::any_cast<long>(value)));
        }
        else if (type == typeid(unsigned long)) {
            lua_pushinteger(L, static_cast<lua_Integer>(std::any_cast<unsigned long>(value)));
        }
        else if (type == typeid(float)) {
            lua_pushnumber(L, std::any_cast<float>(value));
        }
        else if (type == typeid(double)) {
            lua_pushnumber(L, std::any_cast<double>(value));
        }
        else if (type == typeid(std::string) || type == typeid(const char*)) {
            const std::string& str = (type == typeid(std::string)) ? 
                std::any_cast<std::string>(value) : std::any_cast<const char*>(value);
            lua_pushstring(L, str.c_str());
        }
        else {
            // Check if it's an entity or component
            // (In a real implementation, we'd use more robust RTTI)
            try {
                Entity* entity = std::any_cast<Entity*>(value);
                if (entity) {
                    // Create userdata for the entity
                    Entity** udata = static_cast<Entity**>(lua_newuserdata(L, sizeof(Entity*)));
                    *udata = entity;
                    
                    // Set metatable
                    luaL_getmetatable(L, "EntityMetatable");
                    if (lua_isnil(L, -1)) {
                        lua_pop(L, 1);
                        luaL_newmetatable(L, "EntityMetatable");
                    }
                    lua_setmetatable(L, -2);
                    
                    return true;
                }
            }
            catch (...) {
                // Not an entity, try component
                try {
                    Component* component = std::any_cast<Component*>(value);
                    if (component) {
                        // Create userdata for the component
                        Component** udata = static_cast<Component**>(lua_newuserdata(L, sizeof(Component*)));
                        *udata = component;
                        
                        // Set metatable
                        luaL_getmetatable(L, "ComponentMetatable");
                        if (lua_isnil(L, -1)) {
                            lua_pop(L, 1);
                            luaL_newmetatable(L, "ComponentMetatable");
                        }
                        lua_setmetatable(L, -2);
                        
                        return true;
                    }
                }
                catch (...) {
                    // Not a component
                }
            }
            
            // Unsupported type
            lua_pushnil(L);
            return false;
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error pushing value to Lua: " << e.what() << std::endl;
        lua_pushnil(L);
        return false;
    }
    catch (...) {
        std::cerr << "Unknown error pushing value to Lua" << std::endl;
        lua_pushnil(L);
        return false;
    }
}

// Get a C++ value from Lua
std::any LuaScriptEngine::getValueFromLua(lua_State* L, int index) {
    if (!L) {
        return std::any();
    }
    
    try {
        int type = lua_type(L, index);
        
        switch (type) {
            case LUA_TNIL:
                return std::any();
                
            case LUA_TBOOLEAN:
                return static_cast<bool>(lua_toboolean(L, index));
                
            case LUA_TNUMBER:
                if (lua_isinteger(L, index)) {
                    return static_cast<int>(lua_tointeger(L, index));
                }
                else {
                    return lua_tonumber(L, index);
                }
                
            case LUA_TSTRING:
                return std::string(lua_tostring(L, index));
                
            case LUA_TUSERDATA: {
                // Check if it's an entity
                if (luaL_testudata(L, index, "EntityMetatable")) {
                    Entity* entity = *static_cast<Entity**>(lua_touserdata(L, index));
                    return entity;
                }
                // Check if it's a component
                else if (luaL_testudata(L, index, "ComponentMetatable")) {
                    Component* component = *static_cast<Component**>(lua_touserdata(L, index));
                    return component;
                }
                // Unknown userdata
                return std::any();
            }
                
            default:
                return std::any();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error getting value from Lua: " << e.what() << std::endl;
        return std::any();
    }
    catch (...) {
        std::cerr << "Unknown error getting value from Lua" << std::endl;
        return std::any();
    }
}

// Handle Lua result and build ScriptResult
ScriptResult LuaScriptEngine::handleLuaResult(int result, const std::string& context) {
    ScriptResult scriptResult;
    
    if (result != LUA_OK) {
        std::string error = lua_tostring(m_luaState, -1);
        lua_pop(m_luaState, 1);
        
        scriptResult.success = false;
        scriptResult.error = error;
        
        // Extract line number from error message
        size_t linePos = error.find(':', context.length() + 1);
        if (linePos != std::string::npos) {
            size_t lineEnd = error.find(':', linePos + 1);
            if (lineEnd != std::string::npos) {
                try {
                    scriptResult.errorLine = std::stoi(error.substr(linePos + 1, lineEnd - linePos - 1));
                }
                catch (...) {
                    scriptResult.errorLine = -1;
                }
            }
        }
        
        // Call error handler if set
        if (m_errorHandler) {
            m_errorHandler(context, scriptResult.errorLine, scriptResult.error);
        }
    }
    else {
        scriptResult.success = true;
    }
    
    scriptResult.executionTimeMs = 0.0; // Not tracked here
    
    return scriptResult;
}

// Get a sandbox state by name
lua_State* LuaScriptEngine::getSandboxState(const std::string& name) {
    auto it = m_sandboxes.find(name);
    if (it != m_sandboxes.end() && it->second.initialized) {
        return it->second.state;
    }
    
    return nullptr;
}

// Restricted load library for sandboxes
int LuaScriptEngine::restrictedLoadLib(lua_State* L) {
    const char* modname = luaL_checkstring(L, 1);
    
    // Only allow specific modules
    if (strcmp(modname, "string") == 0 ||
        strcmp(modname, "table") == 0 ||
        strcmp(modname, "math") == 0) {
        
        // Push the module name as a global
        lua_getglobal(L, modname);
        return 1;
    }
    
    lua_pushnil(L);
    lua_pushfstring(L, "module '%s' not allowed in sandbox", modname);
    return 2;
}

// Start profiling a function
void LuaScriptEngine::beginProfiling(const std::string& functionName) {
    if (!m_profilingEnabled) {
        return;
    }
    
    m_profilingStartTimes[functionName] = std::chrono::high_resolution_clock::now();
}

// End profiling a function
void LuaScriptEngine::endProfiling(const std::string& functionName) {
    if (!m_profilingEnabled) {
        return;
    }
    
    auto it = m_profilingStartTimes.find(functionName);
    if (it != m_profilingStartTimes.end()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(endTime - it->second).count();
        
        // Update profiling data
        if (m_profilingData.find(functionName) != m_profilingData.end()) {
            m_profilingData[functionName] += elapsed;
        }
        else {
            m_profilingData[functionName] = elapsed;
        }
        
        m_profilingStartTimes.erase(it);
    }
}

} // namespace Scripting