#pragma once

#include "Scripting/IScriptEngine.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <optional>
#include <mutex>
#include <SFML/Graphics.hpp>

class GameSession;

namespace Scripting {

/**
 * @brief Debug command information
 */
struct DebugCommand {
    std::string command;         ///< Command name
    std::string description;     ///< Command description
    std::function<void(const std::vector<std::string>&)> handler; ///< Command handler
};

/**
 * @brief Script variable info for debugging
 */
struct VariableInfo {
    std::string name;            ///< Variable name
    std::string type;            ///< Variable type
    std::string value;           ///< Variable value as string
    int depth = 0;               ///< Nesting depth for table/object members
    bool isTable = false;        ///< Whether this is a table/object
    std::vector<VariableInfo> members; ///< Members for tables/objects
};

/**
 * @brief Script debugger UI and control
 * 
 * This class provides UI and tools for debugging scripts, including
 * breakpoints, stepping, variable inspection, and console.
 */
class ScriptDebugger {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the script debugger
     */
    static ScriptDebugger& getInstance();
    
    /**
     * @brief Initialize the debugger
     * @param gameSession Pointer to the current game session
     * @return True if initialization succeeds
     */
    bool initialize(GameSession* gameSession);
    
    /**
     * @brief Shutdown the debugger
     * @return True if shutdown succeeds
     */
    bool shutdown();
    
    /**
     * @brief Update the debugger
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Draw the debugger UI
     * @param window Render target
     */
    void draw(sf::RenderTarget& window);
    
    /**
     * @brief Toggle the debugger visibility
     */
    void toggleVisibility();
    
    /**
     * @brief Set the active script engine
     * @param engine Script engine to debug
     */
    void setScriptEngine(IScriptEngine* engine);
    
    /**
     * @brief Get the active script engine
     * @return Pointer to active script engine
     */
    IScriptEngine* getScriptEngine() const { return m_scriptEngine; }
    
    /**
     * @brief Add a breakpoint to a script
     * @param scriptName Script file name
     * @param lineNumber Line number for the breakpoint
     * @param condition Optional condition expression
     * @return True if breakpoint was added
     */
    bool addBreakpoint(const std::string& scriptName, int lineNumber, const std::string& condition = "");
    
    /**
     * @brief Remove a breakpoint
     * @param scriptName Script file name
     * @param lineNumber Line number of the breakpoint
     * @return True if breakpoint was removed
     */
    bool removeBreakpoint(const std::string& scriptName, int lineNumber);
    
    /**
     * @brief Enable or disable a breakpoint
     * @param scriptName Script file name
     * @param lineNumber Line number of the breakpoint
     * @param enabled Whether the breakpoint should be enabled
     * @return True if breakpoint was found and updated
     */
    bool enableBreakpoint(const std::string& scriptName, int lineNumber, bool enabled);
    
    /**
     * @brief Get all breakpoints
     * @return Vector of breakpoints
     */
    std::vector<Breakpoint> getBreakpoints() const;
    
    /**
     * @brief Continue execution after hitting a breakpoint
     * @return True if execution continues
     */
    bool continueExecution();
    
    /**
     * @brief Step to next line
     * @return True if step succeeds
     */
    bool stepOver();
    
    /**
     * @brief Step into function
     * @return True if step succeeds
     */
    bool stepInto();
    
    /**
     * @brief Step out of current function
     * @return True if step succeeds
     */
    bool stepOut();
    
    /**
     * @brief Execute a debug command
     * @param command Command text
     * @return Command result as string
     */
    std::string executeDebugCommand(const std::string& command);
    
    /**
     * @brief Get current script execution state
     * @return Debug info if available, nullopt otherwise
     */
    std::optional<ScriptDebugInfo> getCurrentDebugInfo() const;
    
    /**
     * @brief Check if script execution is currently paused
     * @return True if execution is paused at a breakpoint
     */
    bool isPaused() const { return m_isPaused; }
    
    /**
     * @brief Show source code for a specific script
     * @param scriptName Script file name
     * @return True if script was found and loaded
     */
    bool showSource(const std::string& scriptName);
    
    /**
     * @brief Evaluate an expression in the current script context
     * @param expression Expression to evaluate
     * @return Result of the evaluation as string
     */
    std::string evaluateExpression(const std::string& expression);
    
    /**
     * @brief Get local variables in the current script context
     * @return Vector of variable info
     */
    std::vector<VariableInfo> getLocalVariables() const;
    
    /**
     * @brief Get global variables in the current script context
     * @return Vector of variable info
     */
    std::vector<VariableInfo> getGlobalVariables() const;
    
    /**
     * @brief Set a variable value in the current script context
     * @param name Variable name
     * @param value New value as string
     * @return True if variable was set successfully
     */
    bool setVariable(const std::string& name, const std::string& value);
    
    /**
     * @brief Add a custom debug command
     * @param command Command name
     * @param description Command description
     * @param handler Function to handle the command
     * @return True if command was added
     */
    bool addCommand(const std::string& command, 
                  const std::string& description,
                  std::function<void(const std::vector<std::string>&)> handler);
    
    /**
     * @brief Remove a custom debug command
     * @param command Command name
     * @return True if command was removed
     */
    bool removeCommand(const std::string& command);
    
    /**
     * @brief Get all available debug commands
     * @return Map of command names to descriptions
     */
    std::map<std::string, std::string> getCommands() const;
    
    /**
     * @brief Handle keyboard input
     * @param event Keyboard event
     * @return True if event was handled
     */
    bool handleKeyboardEvent(const sf::Event::KeyEvent& event);
    
    /**
     * @brief Set whether the debugger is visible
     * @param visible Whether the debugger should be visible
     */
    void setVisible(bool visible) { m_visible = visible; }
    
    /**
     * @brief Check if the debugger is visible
     * @return True if the debugger is visible
     */
    bool isVisible() const { return m_visible; }
    
    /**
     * @brief Add a log message to the debug console
     * @param message Message text
     * @param type Log message type (info, warning, error)
     */
    void addLogMessage(const std::string& message, const std::string& type = "info");
    
private:
    // Private constructor for singleton
    ScriptDebugger() = default;
    ~ScriptDebugger() = default;
    
    // Prevent copying or moving
    ScriptDebugger(const ScriptDebugger&) = delete;
    ScriptDebugger& operator=(const ScriptDebugger&) = delete;
    ScriptDebugger(ScriptDebugger&&) = delete;
    ScriptDebugger& operator=(ScriptDebugger&&) = delete;
    
    // Breakpoint callback function
    static bool breakpointCallback(IScriptEngine* engine, const ScriptDebugInfo& debugInfo);
    
    // Helper methods for UI
    void drawBreakpointPanel(sf::RenderTarget& window);
    void drawSourcePanel(sf::RenderTarget& window);
    void drawVariablesPanel(sf::RenderTarget& window);
    void drawConsolePanel(sf::RenderTarget& window);
    void drawControlPanel(sf::RenderTarget& window);
    
    // Helper method to parse source code
    bool loadSourceFile(const std::string& filename);
    
    // Helper method to parse command arguments
    std::vector<std::string> parseCommandArgs(const std::string& command);
    
    // Initialize built-in commands
    void initializeCommands();
    
    // Member variables
    GameSession* m_gameSession = nullptr;
    IScriptEngine* m_scriptEngine = nullptr;
    bool m_initialized = false;
    bool m_visible = false;
    bool m_isPaused = false;
    
    // UI state
    sf::Font m_font;
    sf::RectangleShape m_background;
    int m_windowWidth = 0;
    int m_windowHeight = 0;
    
    // Execution state
    std::optional<ScriptDebugInfo> m_currentDebugInfo;
    std::string m_currentScriptName;
    int m_currentLine = -1;
    std::vector<std::string> m_currentSourceLines;
    int m_sourceScrollOffset = 0;
    
    // Breakpoints
    std::vector<Breakpoint> m_breakpoints;
    
    // Debug console
    struct LogMessage {
        std::string text;
        std::string type;
        sf::Color color;
        float time;
    };
    std::vector<LogMessage> m_logMessages;
    std::string m_consoleInput;
    int m_consoleScrollOffset = 0;
    std::vector<std::string> m_commandHistory;
    int m_commandHistoryIndex = -1;
    
    // Debug commands
    std::unordered_map<std::string, DebugCommand> m_commands;
    
    // Thread safety
    mutable std::mutex m_mutex;
};

} // namespace Scripting