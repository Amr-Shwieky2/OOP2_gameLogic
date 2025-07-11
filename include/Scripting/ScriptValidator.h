#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <filesystem>
#include <mutex>
#include <memory>

namespace Scripting {

// Forward declarations
class IScriptEngine;
struct ScriptResult;

/**
 * @brief Script error severity levels
 */
enum class ErrorSeverity {
    Info,       ///< Informational message
    Warning,    ///< Warning (code will run but might have issues)
    Error,      ///< Error (code will not run)
    Critical    ///< Critical error (affects multiple scripts)
};

/**
 * @brief Script error information
 */
struct ScriptError {
    std::string message;      ///< Error message
    std::string scriptName;   ///< Script file name
    int line = -1;            ///< Line number
    int column = -1;          ///< Column number
    ErrorSeverity severity;   ///< Error severity
    std::string code;         ///< Code snippet where the error occurred
    std::string errorType;    ///< Type of error (syntax, reference, etc.)
    std::string suggestion;   ///< Suggestion for fixing the error
};

/**
 * @brief Script validation rules
 */
enum class ValidationRule {
    NoGlobalVariables,    ///< No global variables allowed
    NoEval,               ///< No eval() or similar dynamic code execution
    RequireStrictMode,    ///< Require strict mode (JavaScript)
    RequireLocalDeclarations, ///< Require local variable declarations
    DisallowDeprecatedAPIs,///< Disallow deprecated APIs
    RequireErrorHandling, ///< Require error handling for critical operations
    PerformanceCheck,     ///< Check for performance issues
    StyleCheck            ///< Check for style issues
};

/**
 * @brief Script validation configuration
 */
struct ValidationConfig {
    std::vector<ValidationRule> enabledRules; ///< Enabled validation rules
    bool treatWarningsAsErrors = false;       ///< Whether to treat warnings as errors
    int maxErrorsPerFile = 100;               ///< Maximum errors to report per file
    bool checkAllFiles = false;               ///< Whether to check all files when one changes
    bool validateOnSave = true;               ///< Whether to validate scripts when they're saved
};

/**
 * @brief Script validator for checking script correctness
 * 
 * This class handles validation and error reporting for scripts,
 * helping identify issues before running them.
 */
class ScriptValidator {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the script validator
     */
    static ScriptValidator& getInstance();
    
    /**
     * @brief Initialize the validator
     * @param scriptEngine Script engine to use for validation
     * @return True if initialization succeeds
     */
    bool initialize(IScriptEngine* scriptEngine);
    
    /**
     * @brief Set the script engine to use
     * @param scriptEngine Script engine to use
     */
    void setScriptEngine(IScriptEngine* scriptEngine);
    
    /**
     * @brief Get the script engine being used
     * @return Pointer to the script engine
     */
    IScriptEngine* getScriptEngine() const { return m_scriptEngine; }
    
    /**
     * @brief Validate a script from string
     * @param scriptCode Script code as string
     * @param scriptName Optional name for the script
     * @return Vector of errors found
     */
    std::vector<ScriptError> validateScript(const std::string& scriptCode, const std::string& scriptName = "unnamed");
    
    /**
     * @brief Validate a script file
     * @param filename Path to the script file
     * @return Vector of errors found
     */
    std::vector<ScriptError> validateFile(const std::string& filename);
    
    /**
     * @brief Validate all scripts in a directory
     * @param directory Directory to validate scripts in
     * @param recursive Whether to search recursively
     * @return Map of file names to errors
     */
    std::map<std::string, std::vector<ScriptError>> validateDirectory(
        const std::filesystem::path& directory, bool recursive = true);
    
    /**
     * @brief Get configuration for validation
     * @return Current validation configuration
     */
    ValidationConfig getConfig() const { return m_config; }
    
    /**
     * @brief Set configuration for validation
     * @param config New validation configuration
     */
    void setConfig(const ValidationConfig& config) { m_config = config; }
    
    /**
     * @brief Enable or disable a specific validation rule
     * @param rule Rule to enable or disable
     * @param enabled Whether the rule should be enabled
     */
    void setRuleEnabled(ValidationRule rule, bool enabled);
    
    /**
     * @brief Check if a specific rule is enabled
     * @param rule Rule to check
     * @return True if the rule is enabled
     */
    bool isRuleEnabled(ValidationRule rule) const;
    
    /**
     * @brief Add a custom validation rule
     * @param name Rule name
     * @param validator Function to validate script code
     * @return True if rule was added
     */
    bool addCustomRule(const std::string& name, 
                     std::function<std::vector<ScriptError>(const std::string&, const std::string&)> validator);
    
    /**
     * @brief Remove a custom validation rule
     * @param name Rule name
     * @return True if rule was removed
     */
    bool removeCustomRule(const std::string& name);
    
    /**
     * @brief Get all custom rule names
     * @return Vector of rule names
     */
    std::vector<std::string> getCustomRuleNames() const;
    
    /**
     * @brief Set error callback
     * @param callback Function to call when validation errors are found
     */
    void setErrorCallback(std::function<void(const std::vector<ScriptError>&)> callback);
    
    /**
     * @brief Generate human-readable error message
     * @param error Script error information
     * @return Formatted error message
     */
    static std::string formatErrorMessage(const ScriptError& error);
    
    /**
     * @brief Generate suggestions for fixing an error
     * @param error Script error information
     * @return Suggestion string
     */
    static std::string generateSuggestion(const ScriptError& error);
    
    /**
     * @brief Parse error message from script engine
     * @param result Script execution result
     * @param scriptName Script name
     * @return Script error information
     */
    static ScriptError parseEngineError(const ScriptResult& result, const std::string& scriptName);
    
    /**
     * @brief Export validation results to a report file
     * @param filename File to write the report to
     * @param errors Errors to include in the report
     * @return True if report was written successfully
     */
    bool exportValidationReport(const std::string& filename, 
                              const std::map<std::string, std::vector<ScriptError>>& errors);
    
    /**
     * @brief Get statistics about the last validation run
     * @return Map of statistic names to values
     */
    std::map<std::string, int> getValidationStats() const;
    
    /**
     * @brief Monitor a file for changes and validate when it changes
     * @param filename File to monitor
     * @param validateOnChange Whether to validate when the file changes
     * @return True if file monitoring was set up
     */
    bool monitorFile(const std::string& filename, bool validateOnChange = true);
    
    /**
     * @brief Stop monitoring a file
     * @param filename File to stop monitoring
     * @return True if file was removed from monitoring
     */
    bool stopMonitoringFile(const std::string& filename);
    
    /**
     * @brief Check if a file is being monitored
     * @param filename File to check
     * @return True if file is being monitored
     */
    bool isFileMonitored(const std::string& filename) const;
    
    /**
     * @brief Get a list of all monitored files
     * @return Vector of monitored file paths
     */
    std::vector<std::filesystem::path> getMonitoredFiles() const;
    
    /**
     * @brief Check for changes in monitored files
     */
    void checkMonitoredFiles();
    
private:
    // Private constructor for singleton
    ScriptValidator() = default;
    ~ScriptValidator() = default;
    
    // Prevent copying or moving
    ScriptValidator(const ScriptValidator&) = delete;
    ScriptValidator& operator=(const ScriptValidator&) = delete;
    ScriptValidator(ScriptValidator&&) = delete;
    ScriptValidator& operator=(ScriptValidator&&) = delete;
    
    // Rule implementations
    std::vector<ScriptError> checkGlobalVariables(const std::string& scriptCode, const std::string& scriptName);
    std::vector<ScriptError> checkEvalUsage(const std::string& scriptCode, const std::string& scriptName);
    std::vector<ScriptError> checkStrictMode(const std::string& scriptCode, const std::string& scriptName);
    std::vector<ScriptError> checkLocalDeclarations(const std::string& scriptCode, const std::string& scriptName);
    std::vector<ScriptError> checkDeprecatedAPIs(const std::string& scriptCode, const std::string& scriptName);
    std::vector<ScriptError> checkErrorHandling(const std::string& scriptCode, const std::string& scriptName);
    std::vector<ScriptError> checkPerformanceIssues(const std::string& scriptCode, const std::string& scriptName);
    std::vector<ScriptError> checkStyleIssues(const std::string& scriptCode, const std::string& scriptName);
    
    // Helper methods
    std::vector<ScriptError> runBuiltInRules(const std::string& scriptCode, const std::string& scriptName);
    std::vector<ScriptError> runCustomRules(const std::string& scriptCode, const std::string& scriptName);
    std::vector<std::string> readScriptLines(const std::string& scriptCode);
    
    // Member variables
    IScriptEngine* m_scriptEngine = nullptr;
    ValidationConfig m_config;
    
    // Custom rules
    std::map<std::string, std::function<std::vector<ScriptError>(const std::string&, const std::string&)>> m_customRules;
    
    // Error callback
    std::function<void(const std::vector<ScriptError>&)> m_errorCallback;
    
    // Validation statistics
    struct ValidationStats {
        int totalFilesChecked = 0;
        int totalErrors = 0;
        int totalWarnings = 0;
        int totalInfos = 0;
        int filesWithErrors = 0;
        int fixedErrors = 0;
    };
    ValidationStats m_stats;
    
    // File monitoring
    struct MonitoredFile {
        std::filesystem::path path;
        std::filesystem::file_time_type lastWriteTime;
        bool validateOnChange;
    };
    std::map<std::string, MonitoredFile> m_monitoredFiles;
    
    // Thread safety
    mutable std::mutex m_mutex;
};

/**
 * @brief Helper to convert rule enum to string
 * @param rule Validation rule
 * @return String representation of the rule
 */
inline std::string ruleToString(ValidationRule rule) {
    switch (rule) {
        case ValidationRule::NoGlobalVariables: return "NoGlobalVariables";
        case ValidationRule::NoEval: return "NoEval";
        case ValidationRule::RequireStrictMode: return "RequireStrictMode";
        case ValidationRule::RequireLocalDeclarations: return "RequireLocalDeclarations";
        case ValidationRule::DisallowDeprecatedAPIs: return "DisallowDeprecatedAPIs";
        case ValidationRule::RequireErrorHandling: return "RequireErrorHandling";
        case ValidationRule::PerformanceCheck: return "PerformanceCheck";
        case ValidationRule::StyleCheck: return "StyleCheck";
        default: return "Unknown";
    }
}

/**
 * @brief Helper to convert error severity to string
 * @param severity Error severity
 * @return String representation of the severity
 */
inline std::string severityToString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Info: return "Info";
        case ErrorSeverity::Warning: return "Warning";
        case ErrorSeverity::Error: return "Error";
        case ErrorSeverity::Critical: return "Critical";
        default: return "Unknown";
    }
}

} // namespace Scripting