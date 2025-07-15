#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <memory>
#include <format>
#include <source_location>
#include <chrono>

namespace GameExceptions {

/**
 * @brief Logging levels to categorize messages
 */
enum class LogLevel {
    Debug,    // Detailed information, typically useful only for diagnostics
    Info,     // General information about program execution
    Warning,  // Potential issues that don't prevent execution
    Error,    // Errors that allow the program to continue
    Critical  // Severe errors that might cause the program to terminate
};

/**
 * @brief Abstract base class for log output targets
 */
class LogTarget {
public:
    virtual ~LogTarget() = default;
    virtual void write(LogLevel level, const std::string& message) = 0;
    virtual void flush() = 0;
};

/**
 * @brief Log target that outputs to the console
 */
class ConsoleLogTarget : public LogTarget {
public:
    void write(LogLevel level, const std::string& message) override;
    void flush() override;
};

/**
 * @brief Log target that outputs to a file
 */
class FileLogTarget : public LogTarget {
public:
    explicit FileLogTarget(const std::string& filename);
    ~FileLogTarget() override;
    
    void write(LogLevel level, const std::string& message) override;
    void flush() override;
    
private:
    std::ofstream m_file;
};

/**
 * @brief Main logging class that manages log targets and formats messages
 */
class Logger {
public:
    static Logger& getInstance();

    // Prevent copying or moving
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // Add/remove log targets
    void addTarget(std::unique_ptr<LogTarget> target);
    void clearTargets();

    // Set minimum log level
    void setMinLevel(LogLevel level) { m_minLevel = level; }
    LogLevel getMinLevel() const { return m_minLevel; }

    // Log methods with source location
    void log(LogLevel level, 
             const std::string& message, 
             const std::source_location& location = std::source_location::current());

    // Convenience methods for different log levels
    void debug(const std::string& message, 
               const std::source_location& location = std::source_location::current());
    void info(const std::string& message, 
              const std::source_location& location = std::source_location::current());
    void warning(const std::string& message, 
                const std::source_location& location = std::source_location::current());
    void error(const std::string& message, 
              const std::source_location& location = std::source_location::current());
    void critical(const std::string& message, 
                 const std::source_location& location = std::source_location::current());

    // Log exceptions
    void logException(const std::exception& ex, 
                     LogLevel level = LogLevel::Error,
                     const std::source_location& location = std::source_location::current());

    // Convert log level to string
    static std::string logLevelToString(LogLevel level);

private:
    Logger();  // Private constructor for singleton

    std::string formatLogMessage(LogLevel level, 
                               const std::string& message, 
                               const std::source_location& location);

    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);

    // Thread safety
    std::mutex m_mutex;
    
    // Log targets
    std::vector<std::unique_ptr<LogTarget>> m_targets;
    
    // Minimum log level to output
    LogLevel m_minLevel = LogLevel::Debug;
};

// Global function for easy logging
inline Logger& getLogger() {
    return Logger::getInstance();
}

} // namespace GameExceptions