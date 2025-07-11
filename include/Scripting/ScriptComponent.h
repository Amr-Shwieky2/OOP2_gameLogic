#pragma once

#include "Component.h"
#include "Scripting/IScriptEngine.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Scripting {

/**
 * @brief Component that adds scripted behavior to an entity
 * 
 * This component allows attaching scripts to entities to define
 * custom behavior through scripts instead of C++ code.
 */
class ScriptComponent : public Component {
public:
    /**
     * @brief Constructor
     * @param scriptEngine The script engine to use (nullptr to use the default engine)
     */
    explicit ScriptComponent(IScriptEngine* scriptEngine = nullptr);
    
    /**
     * @brief Destructor
     */
    ~ScriptComponent() override;

    /**
     * @brief Load a script from file
     * @param filename Path to the script file
     * @return True if the script was loaded successfully
     */
    bool loadScript(const std::string& filename);

    /**
     * @brief Load a script from string
     * @param scriptCode The script code as a string
     * @param scriptName Optional name for the script (for debugging)
     * @return True if the script was loaded successfully
     */
    bool loadScriptFromString(const std::string& scriptCode, const std::string& scriptName = "unnamed");
    
    /**
     * @brief Add a script source to the component
     * @param filename Path to the script file
     * @return True if the script was added successfully
     */
    bool addScriptSource(const std::string& filename);
    
    /**
     * @brief Remove a script source from the component
     * @param filename Path to the script file
     * @return True if the script was removed
     */
    bool removeScriptSource(const std::string& filename);
    
    /**
     * @brief Get all script sources attached to this component
     * @return Vector of script filenames
     */
    std::vector<std::string> getScriptSources() const;

    /**
     * @brief Set whether to use sandbox mode for this component's scripts
     * @param enabled Whether sandbox mode should be enabled
     */
    void setSandboxMode(bool enabled);

    /**
     * @brief Check if sandbox mode is enabled for this component
     * @return True if sandbox mode is enabled
     */
    bool isSandboxModeEnabled() const;

    /**
     * @brief Call a specific function in the attached scripts
     * @param functionName Name of the function to call
     * @param args Arguments to pass to the function
     * @return Result from the last script that defines the function
     */
    ScriptResult callFunction(const std::string& functionName, const std::vector<std::any>& args = {});

    /**
     * @brief Set a script variable value
     * @param name Name of the variable
     * @param value Value to set
     * @return True if setting succeeds
     */
    bool setVariable(const std::string& name, const std::any& value);

    /**
     * @brief Get a script variable value
     * @param name Name of the variable
     * @return Value of the variable if it exists
     */
    std::optional<std::any> getVariable(const std::string& name);

    /**
     * @brief Enable or disable auto-reloading of scripts when files change
     * @param enabled Whether auto-reload should be enabled
     */
    void setAutoReload(bool enabled);

    /**
     * @brief Check if auto-reload is enabled
     * @return True if auto-reload is enabled
     */
    bool isAutoReloadEnabled() const;

    /**
     * @brief Manually reload all scripts
     * @return True if all scripts reloaded successfully
     */
    bool reloadScripts();
    
    /**
     * @brief Get the script engine used by this component
     * @return Pointer to the script engine
     */
    IScriptEngine* getScriptEngine() const;

    // Component interface overrides
    void initialize() override;
    void update(float deltaTime) override;
    void onDestroy() override;
    Component* clone() const override;

    // Custom serialization and inspection support
    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;
    void inspect() override;

    /**
     * @brief Get the component's type name
     * @return Type name string
     */
    static std::string getStaticTypeName() { return "ScriptComponent"; }

    /**
     * @brief Get the component's type name
     * @return Type name string
     */
    std::string getTypeName() const override { return getStaticTypeName(); }

private:
    /**
     * @brief Check if any script files have been modified
     * @return True if any scripts need reloading
     */
    bool checkScriptFilesForChanges();

    /**
     * @brief Expose entity and component API to scripts
     */
    void exposeAPI();
    
    /**
     * @brief Call a lifecycle method in all scripts that define it
     * @param methodName Name of the method to call
     * @param args Arguments to pass to the method
     */
    void callLifecycleMethod(const std::string& methodName, const std::vector<std::any>& args = {});

    IScriptEngine* m_scriptEngine;
    std::vector<std::string> m_scriptSources;
    std::unordered_map<std::string, std::time_t> m_scriptModificationTimes;
    bool m_sandboxMode = false;
    bool m_autoReload = false;
    bool m_initialized = false;
    std::string m_sandboxName;
    
    // Script profiling data
    bool m_profilingEnabled = false;
    std::unordered_map<std::string, double> m_profilingData;
    
    // Script error handling
    bool m_hadError = false;
    std::string m_lastError;
    int m_lastErrorLine = -1;
};

} // namespace Scripting