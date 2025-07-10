// Exception.h - Base exception classes for the game
#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <source_location>
#include <chrono>
#include <format>

namespace GameExceptions {

/**
 * @brief Base exception class for all game exceptions
 * 
 * Features:
 * - Captures source location (file, line, function)
 * - Records timestamp
 * - Stackable error context
 * - Error codes
 * - Severity levels
 */
class Exception : public std::exception {
public:
    // Severity levels for exceptions
    enum class Severity {
        Info,       // Informational, not critical
        Warning,    // Potentially problematic but recoverable
        Error,      // Error that might be recoverable
        Critical,   // Critical error that likely requires termination
        Fatal       // Fatal error that requires immediate termination
    };

    // Constructor with automatic source location
    Exception(const std::string& message, 
              Severity severity = Severity::Error,
              int errorCode = 0,
              const std::source_location& location = std::source_location::current())
        : m_message(message),
          m_severity(severity),
          m_errorCode(errorCode),
          m_timestamp(std::chrono::system_clock::now()),
          m_file(location.file_name()),
          m_line(location.line()),
          m_function(location.function_name()) {
    }

    // Constructor from another exception (for wrapping)
    Exception(const std::string& message,
              const std::exception& innerException,
              Severity severity = Severity::Error,
              int errorCode = 0,
              const std::source_location& location = std::source_location::current())
        : m_message(message),
          m_severity(severity),
          m_errorCode(errorCode),
          m_timestamp(std::chrono::system_clock::now()),
          m_file(location.file_name()),
          m_line(location.line()),
          m_function(location.function_name()) {
        
        m_context.push_back(innerException.what());
    }

    // Add context to the exception (for building exception chains)
    void addContext(const std::string& context) {
        m_context.push_back(context);
    }

    // Override what() to include all information
    const char* what() const noexcept override {
        try {
            // Compute the full message on demand
            m_fullMessage = formatExceptionMessage();
            return m_fullMessage.c_str();
        }
        catch (...) {
            // Failsafe if we can't generate the message
            return "Error formatting exception message";
        }
    }

    // Getters
    Severity getSeverity() const { return m_severity; }
    int getErrorCode() const { return m_errorCode; }
    std::string getMessage() const { return m_message; }
    std::string getFile() const { return m_file; }
    int getLine() const { return m_line; }
    std::string getFunction() const { return m_function; }
    std::chrono::system_clock::time_point getTimestamp() const { return m_timestamp; }
    const std::vector<std::string>& getContext() const { return m_context; }
    
    // Helper methods
    static std::string severityToString(Severity severity) {
        switch (severity) {
            case Severity::Info: return "INFO";
            case Severity::Warning: return "WARNING";
            case Severity::Error: return "ERROR";
            case Severity::Critical: return "CRITICAL";
            case Severity::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }

private:
    std::string formatExceptionMessage() const {
        std::string result = std::format("[{}] {}: {} (Code: {})\n  at {}:{} in {}", 
                                        severityToString(m_severity), 
                                        formatTimestamp(m_timestamp),
                                        m_message, 
                                        m_errorCode,
                                        m_file, 
                                        m_line, 
                                        m_function);
        
        if (!m_context.empty()) {
            result += "\n  Context:";
            for (const auto& ctx : m_context) {
                result += "\n  - " + ctx;
            }
        }
        
        return result;
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) const {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::tm tm_time;
#ifdef _MSC_VER
        localtime_s(&tm_time, &time);
#else
        tm_time = *std::localtime(&time);
#endif
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_time);
        return buffer;
    }

    std::string m_message;                                 // Main error message
    Severity m_severity;                                   // Error severity
    int m_errorCode;                                       // Error code
    std::chrono::system_clock::time_point m_timestamp;     // When the exception occurred
    std::string m_file;                                    // Source file
    int m_line;                                            // Line number
    std::string m_function;                                // Function name
    std::vector<std::string> m_context;                    // Additional context info
    mutable std::string m_fullMessage;                     // Cached full message
};

} // namespace GameExceptions