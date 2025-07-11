#include "Scripting/ScriptComponent.h"
#include "Scripting/ScriptManager.h"
#include "Entity.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

namespace Scripting {

ScriptComponent::ScriptComponent(IScriptEngine* scriptEngine)
    : Component()
    , m_scriptEngine(scriptEngine)
    , m_sandboxMode(false)
    , m_autoReload(false)
    , m_initialized(false)
    , m_profilingEnabled(false)
    , m_hadError(false)
{
    // If no engine was provided, use the default from the script manager
    if (!m_scriptEngine) {
        m_scriptEngine = ScriptManager::getInstance().getDefaultEngine();
    }

    // Create a unique sandbox name based on this component's address
    std::ostringstream ss;
    ss << "ScriptComponent_" << this;
    m_sandboxName = ss.str();
}

ScriptComponent::~ScriptComponent()
{
    // Clean up resources
    onDestroy();
}

bool ScriptComponent::loadScript(const std::string& filename)
{
    if (!m_scriptEngine) {
        m_lastError = "No script engine available";
        m_hadError = true;
        return false;
    }

    // Check if file exists
    if (!std::filesystem::exists(filename)) {
        m_lastError = "Script file not found: " + filename;
        m_hadError = true;
        return false;
    }

    // Add to script sources if not already present
    if (std::find(m_scriptSources.begin(), m_scriptSources.end(), filename) == m_scriptSources.end()) {
        m_scriptSources.push_back(filename);

        // Store last modification time for hot reloading
        m_scriptModificationTimes[filename] = std::filesystem::last_write_time(filename);
    }

    // Execute the script
    ScriptResult result;
    if (m_sandboxMode && !m_sandboxName.empty()) {
        // Ensure sandbox exists
        m_scriptEngine->createSandbox(m_sandboxName);

        // Read script file
        std::ifstream file(filename);
        if (!file.is_open()) {
            m_lastError = "Failed to open script file: " + filename;
            m_hadError = true;
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        // Execute in sandbox
        result = m_scriptEngine->executeInSandbox(m_sandboxName, buffer.str(), filename);
    }
    else {
        // Execute directly
        result = m_scriptEngine->executeFile(filename);
    }

    if (!result.success) {
        m_lastError = "Error in script " + filename + " at line " + std::to_string(result.errorLine) + ": " + result.error;
        m_hadError = true;
        std::cerr << m_lastError << std::endl;
        return false;
    }

    return true;
}

bool ScriptComponent::loadScriptFromString(const std::string& scriptCode, const std::string& scriptName)
{
    if (!m_scriptEngine) {
        m_lastError = "No script engine available";
        m_hadError = true;
        return false;
    }

    // Execute the script
    ScriptResult result;
    if (m_sandboxMode && !m_sandboxName.empty()) {
        // Ensure sandbox exists
        m_scriptEngine->createSandbox(m_sandboxName);

        // Execute in sandbox
        result = m_scriptEngine->executeInSandbox(m_sandboxName, scriptCode, scriptName);
    }
    else {
        // Execute directly
        result = m_scriptEngine->executeString(scriptCode, scriptName);
    }

    if (!result.success) {
        m_lastError = "Error in script " + scriptName + " at line " + std::to_string(result.errorLine) + ": " + result.error;
        m_hadError = true;
        std::cerr << m_lastError << std::endl;
        return false;
    }

    return true;
}

bool ScriptComponent::addScriptSource(const std::string& filename)
{
    if (std::find(m_scriptSources.begin(), m_scriptSources.end(), filename) != m_scriptSources.end()) {
        // Already in the list
        return true;
    }

    m_scriptSources.push_back(filename);

    // Store last modification time
    if (std::filesystem::exists(filename)) {
        m_scriptModificationTimes[filename] = std::filesystem::last_write_time(filename);
    }

    // If component is already initialized, load the script right away
    if (m_initialized) {
        return loadScript(filename);
    }

    return true;
}

bool ScriptComponent::removeScriptSource(const std::string& filename)
{
    auto it = std::find(m_scriptSources.begin(), m_scriptSources.end(), filename);
    if (it == m_scriptSources.end()) {
        return false; // Not found
    }

    m_scriptSources.erase(it);
    m_scriptModificationTimes.erase(filename);

    return true;
}

std::vector<std::string> ScriptComponent::getScriptSources() const
{
    return m_scriptSources;
}

void ScriptComponent::setSandboxMode(bool enabled)
{
    m_sandboxMode = enabled;

    // If we're turning on sandbox mode and we're already initialized,
    // we need to reload all scripts in the sandbox
    if (enabled && m_initialized) {
        if (m_scriptEngine) {
            m_scriptEngine->createSandbox(m_sandboxName);
            reloadScripts();
        }
    }
}

bool ScriptComponent::isSandboxModeEnabled() const
{
    return m_sandboxMode;
}

ScriptResult ScriptComponent::callFunction(const std::string& functionName, const std::vector<std::any>& args)
{
    if (!m_scriptEngine) {
        ScriptResult result;
        result.success = false;
        result.error = "No script engine available";
        return result;
    }

    // Call the function
    return m_scriptEngine->callFunction(functionName, args);
}

bool ScriptComponent::setVariable(const std::string& name, const std::any& value)
{
    if (!m_scriptEngine) {
        return false;
    }

    return m_scriptEngine->setGlobal(name, value);
}

std::optional<std::any> ScriptComponent::getVariable(const std::string& name)
{
    if (!m_scriptEngine) {
        return std::nullopt;
    }

    return m_scriptEngine->getGlobal(name);
}

void ScriptComponent::setAutoReload(bool enabled)
{
    m_autoReload = enabled;
}

bool ScriptComponent::isAutoReloadEnabled() const
{
    return m_autoReload;
}

bool ScriptComponent::reloadScripts()
{
    bool allSucceeded = true;

    // Reload all scripts
    for (const auto& script : m_scriptSources) {
        if (!loadScript(script)) {
            allSucceeded = false;
        }
    }

    return allSucceeded;
}

IScriptEngine* ScriptComponent::getScriptEngine() const
{
    return m_scriptEngine;
}

void ScriptComponent::initialize()
{
    if (m_initialized) {
        return;
    }

    // Create sandbox if needed
    if (m_sandboxMode && m_scriptEngine) {
        m_scriptEngine->createSandbox(m_sandboxName);
    }

    // Expose API to script
    exposeAPI();

    // Load all scripts
    for (const auto& script : m_scriptSources) {
        loadScript(script);
    }

    // Call the initialize lifecycle method if it exists
    callLifecycleMethod("initialize");

    m_initialized = true;
}

void ScriptComponent::update(float deltaTime)
{
    // Check for script file changes if auto-reload is enabled
    if (m_autoReload) {
        if (checkScriptFilesForChanges()) {
            reloadScripts();
        }
    }

    // Call the update lifecycle method if it exists
    callLifecycleMethod("update", {deltaTime});
}

void ScriptComponent::onDestroy()
{
    // Call the onDestroy lifecycle method if it exists
    if (m_initialized) {
        callLifecycleMethod("onDestroy");
    }
}

Component* ScriptComponent::clone() const
{
    ScriptComponent* clone = new ScriptComponent(m_scriptEngine);
    
    clone->m_scriptSources = m_scriptSources;
    clone->m_scriptModificationTimes = m_scriptModificationTimes;
    clone->m_sandboxMode = m_sandboxMode;
    clone->m_autoReload = m_autoReload;
    clone->m_sandboxName = m_sandboxName + "_clone";
    clone->m_profilingEnabled = m_profilingEnabled;
    
    return clone;
}

void ScriptComponent::serialize(std::ostream& os) const
{
    // Write component type
    os << "ScriptComponent\n";
    
    // Write number of script sources
    os << m_scriptSources.size() << "\n";
    
    // Write each script source
    for (const auto& source : m_scriptSources) {
        os << source << "\n";
    }
    
    // Write settings
    os << (m_sandboxMode ? "1" : "0") << "\n";
    os << (m_autoReload ? "1" : "0") << "\n";
    os << (m_profilingEnabled ? "1" : "0") << "\n";
}

void ScriptComponent::deserialize(std::istream& is)
{
    // Read component type (already read by the entity)
    
    // Read number of script sources
    size_t sourceCount;
    is >> sourceCount;
    is.ignore(); // Skip newline
    
    // Read each script source
    m_scriptSources.clear();
    for (size_t i = 0; i < sourceCount; ++i) {
        std::string source;
        std::getline(is, source);
        m_scriptSources.push_back(source);
    }
    
    // Read settings
    std::string value;
    std::getline(is, value);
    m_sandboxMode = (value == "1");
    
    std::getline(is, value);
    m_autoReload = (value == "1");
    
    std::getline(is, value);
    m_profilingEnabled = (value == "1");
}

void ScriptComponent::inspect()
{
    // This would be implemented with your UI framework
    // For example, with ImGui:
    /*
    if (ImGui::CollapsingHeader("ScriptComponent")) {
        // Display script sources
        if (ImGui::TreeNode("Script Sources")) {
            for (size_t i = 0; i < m_scriptSources.size(); ++i) {
                ImGui::Text("%s", m_scriptSources[i].c_str());
                if (ImGui::Button(("Remove##" + std::to_string(i)).c_str())) {
                    removeScriptSource(m_scriptSources[i]);
                    ImGui::TreePop();
                    return;
                }
            }
            
            // Add new script
            static char newScriptPath[256] = "";
            ImGui::InputText("New Script", newScriptPath, sizeof(newScriptPath));
            if (ImGui::Button("Add Script")) {
                if (newScriptPath[0] != '\0') {
                    addScriptSource(newScriptPath);
                    newScriptPath[0] = '\0';
                }
            }
            
            ImGui::TreePop();
        }
        
        // Settings
        bool sandbox = m_sandboxMode;
        if (ImGui::Checkbox("Sandbox Mode", &sandbox)) {
            setSandboxMode(sandbox);
        }
        
        bool autoReload = m_autoReload;
        if (ImGui::Checkbox("Auto Reload", &autoReload)) {
            setAutoReload(autoReload);
        }
        
        bool profiling = m_profilingEnabled;
        if (ImGui::Checkbox("Enable Profiling", &profiling)) {
            m_profilingEnabled = profiling;
            if (m_scriptEngine) {
                m_scriptEngine->enableProfiling(profiling);
            }
        }
        
        // Actions
        if (ImGui::Button("Reload Scripts")) {
            reloadScripts();
        }
        
        // Display error if any
        if (m_hadError) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: %s", m_lastError.c_str());
        }
    }
    */
}

bool ScriptComponent::checkScriptFilesForChanges()
{
    bool changed = false;
    
    for (const auto& script : m_scriptSources) {
        try {
            // Get current modification time
            if (!std::filesystem::exists(script)) {
                continue;
            }
            
            auto currentModTime = std::filesystem::last_write_time(script);
            
            // Check if this file has been modified
            auto it = m_scriptModificationTimes.find(script);
            if (it != m_scriptModificationTimes.end()) {
                if (currentModTime != it->second) {
                    it->second = currentModTime;
                    changed = true;
                }
            }
            else {
                // New file
                m_scriptModificationTimes[script] = currentModTime;
                changed = true;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error checking script file " << script << ": " << e.what() << std::endl;
        }
    }
    
    return changed;
}

void ScriptComponent::exposeAPI()
{
    if (!m_scriptEngine || !getOwner()) {
        return;
    }
    
    // Expose the entity to the script
    m_scriptEngine->exposeEntity(getOwner(), "this");
    
    // Set up some basic properties
    setVariable("entityId", static_cast<int>(getOwner()->getId()));
    
    // Expose owner's components
    auto components = getOwner()->getAllComponents();
    for (Component* component : components) {
        if (component == this) {
            continue; // Skip self
        }
        
        // Set component by type name
        std::string typeName = component->getTypeName();
        setVariable(typeName, component);
    }
    
    // Add helper function to get components by type
    m_scriptEngine->registerFunction("getComponent", [this](const std::vector<std::any>& args) -> std::any {
        if (args.empty() || !getOwner()) {
            return nullptr;
        }
        
        try {
            std::string componentName = std::any_cast<std::string>(args[0]);
            return getOwner()->operator[](componentName);
        }
        catch (const std::exception& e) {
            std::cerr << "Error in getComponent: " << e.what() << std::endl;
            return nullptr;
        }
    });
}

void ScriptComponent::callLifecycleMethod(const std::string& methodName, const std::vector<std::any>& args)
{
    if (!m_scriptEngine) {
        return;
    }
    
    // Start profiling if enabled
    auto startTime = m_profilingEnabled ? 
        std::chrono::high_resolution_clock::now() : 
        std::chrono::high_resolution_clock::time_point();
    
    // Call the method if it exists
    ScriptResult result = m_scriptEngine->callFunction(methodName, args);
    
    // Record profiling data if enabled
    if (m_profilingEnabled) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        m_profilingData[methodName] = elapsed;
    }
    
    // Handle errors
    if (!result.success) {
        m_lastError = "Error in " + methodName + ": " + result.error;
        m_hadError = true;
        std::cerr << m_lastError << std::endl;
    }
}

} // namespace Scripting