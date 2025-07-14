#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <memory>

/**
 * @brief Responsible for logging Settings screen events only
 * Single Responsibility: Event logging and debugging information
 */
class SettingsEventLogger {
public:
    enum class LogLevel {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    SettingsEventLogger();
    ~SettingsEventLogger();

    /**
     * @brief Log different events
     */
    void logCommandExecution(const std::string& commandName, bool success);
    void logInputEvent(const sf::Event& event);
    void logAutoSaveAction(const std::string& action);
    void logError(const std::string& operation, const std::string& error);

    /**
     * @brief Manage log file
     */
    void enableFileLogging(bool enable);
    void enableConsoleLogging(bool enable);
    void setLogLevel(LogLevel level);

    /**
     * @brief Print diagnostic information
     */
    void printSettingsState();
    void printCommandHistory();

private:
    bool m_fileLoggingEnabled = false;
    bool m_consoleLoggingEnabled = true;
    LogLevel m_currentLogLevel = LogLevel::INFO;

    std::unique_ptr<std::ofstream> m_logFile;

    /**
     * @brief Helper methods for logging
     */
    void writeLog(LogLevel level, const std::string& category, const std::string& message);
    void openLogFile();
    void closeLogFile();

    std::string getCurrentTimestamp() const;
    std::string logLevelToString(LogLevel level) const;
    std::string eventTypeToString(sf::Event::EventType type) const;
    std::string keyCodeToString(sf::Keyboard::Key key) const;

    /**
     * @brief Console output helpers
     */
    void printToConsole(LogLevel level, const std::string& category, const std::string& message);
    void printToFile(LogLevel level, const std::string& category, const std::string& message);
};