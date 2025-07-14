#include "../Settings/SettingsEventLogger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>  // for localtime_s and localtime_r

SettingsEventLogger::SettingsEventLogger() {
    std::cout << "SettingsEventLogger: Initialized for event logging only" << std::endl;
}

SettingsEventLogger::~SettingsEventLogger() {
    try {
        std::cout << "SettingsEventLogger: Starting safe destruction..." << std::endl;
        closeLogFile();
        std::cout << "SettingsEventLogger: Destroyed and log file closed safely" << std::endl;
    }
    catch (...) {
        // Silent cleanup - don't throw in destructor
    }
}

void SettingsEventLogger::logCommandExecution(const std::string& commandName, bool success) {
    std::string status = success ? "SUCCESS" : "FAILED";
    writeLog(LogLevel::INFO, "COMMAND", commandName + " - " + status);
}

void SettingsEventLogger::logInputEvent(const sf::Event& event) {
    if (m_currentLogLevel > LogLevel::DEBUG) {
        return; // Skip if debug logging is disabled
    }

    std::string eventInfo = "Type: " + eventTypeToString(event.type);

    if (event.type == sf::Event::KeyPressed) {
        eventInfo += ", Key: " + keyCodeToString(event.key.code);
        if (event.key.control) eventInfo += " [Ctrl]";
        if (event.key.alt) eventInfo += " [Alt]";
        if (event.key.shift) eventInfo += " [Shift]";
    }

    writeLog(LogLevel::DEBUG, "INPUT", eventInfo);
}

void SettingsEventLogger::logAutoSaveAction(const std::string& action) {
    writeLog(LogLevel::INFO, "AUTO-SAVE", action);
}

void SettingsEventLogger::logError(const std::string& operation, const std::string& error) {
    writeLog(LogLevel::ERROR, operation, error);
}

void SettingsEventLogger::enableFileLogging(bool enable) {
    m_fileLoggingEnabled = enable;

    if (enable) {
        openLogFile();
        writeLog(LogLevel::INFO, "LOGGER", "File logging enabled");
    }
    else {
        writeLog(LogLevel::INFO, "LOGGER", "File logging disabled");
        closeLogFile();
    }
}

void SettingsEventLogger::enableConsoleLogging(bool enable) {
    m_consoleLoggingEnabled = enable;
    std::cout << "SettingsEventLogger: Console logging "
        << (enable ? "enabled" : "disabled") << std::endl;
}

void SettingsEventLogger::setLogLevel(LogLevel level) {
    m_currentLogLevel = level;
    writeLog(LogLevel::INFO, "LOGGER", "Log level set to " + logLevelToString(level));
}

void SettingsEventLogger::printSettingsState() {
    std::cout << "\n" << std::string(40, '=') << std::endl;
    std::cout << "    SETTINGS EVENT LOGGER STATE" << std::endl;
    std::cout << std::string(40, '=') << std::endl;

    std::cout << "File Logging: " << (m_fileLoggingEnabled ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Console Logging: " << (m_consoleLoggingEnabled ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Current Log Level: " << logLevelToString(m_currentLogLevel) << std::endl;

    std::cout << std::string(40, '=') << "\n" << std::endl;
}

void SettingsEventLogger::printCommandHistory() {
    writeLog(LogLevel::INFO, "LOGGER", "Command history requested - delegating to CommandInvoker");
}

void SettingsEventLogger::writeLog(LogLevel level, const std::string& category, const std::string& message) {
    // Check if we should log this level
    if (level < m_currentLogLevel) {
        return;
    }

    // Log to console if enabled
    if (m_consoleLoggingEnabled) {
        printToConsole(level, category, message);
    }

    // Log to file if enabled
    if (m_fileLoggingEnabled && m_logFile && m_logFile->is_open()) {
        printToFile(level, category, message);
    }
}

void SettingsEventLogger::openLogFile() {
    try {
        m_logFile = std::make_unique<std::ofstream>("settings_log.txt", std::ios::app);
        if (m_logFile->is_open()) {
            *m_logFile << "\n" << std::string(50, '-') << std::endl;
            *m_logFile << "Settings Event Logger Session Started: " << getCurrentTimestamp() << std::endl;
            *m_logFile << std::string(50, '-') << std::endl;
        }
        else {
            std::cout << "SettingsEventLogger: Failed to open log file" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cout << "SettingsEventLogger: Error opening log file: " << e.what() << std::endl;
    }
}

void SettingsEventLogger::closeLogFile() {
    if (m_logFile && m_logFile->is_open()) {
        *m_logFile << "Settings Event Logger Session Ended: " << getCurrentTimestamp() << std::endl;
        *m_logFile << std::string(50, '-') << "\n" << std::endl;
        m_logFile->close();
    }
}

std::string SettingsEventLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;

#ifdef _WIN32
    // Windows: use localtime_s for safety
    std::tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
#else
    // Unix/Linux: localtime_r is thread-safe
    std::tm timeinfo;
    localtime_r(&time_t, &timeinfo);
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
#endif

    return ss.str();
}

std::string SettingsEventLogger::logLevelToString(LogLevel level) const {
    switch (level) {
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR: return "ERROR";
    case LogLevel::DEBUG: return "DEBUG";
    default: return "UNKNOWN";
    }
}

std::string SettingsEventLogger::eventTypeToString(sf::Event::EventType type) const {
    switch (type) {
    case sf::Event::KeyPressed: return "KeyPressed";
    case sf::Event::KeyReleased: return "KeyReleased";
    case sf::Event::MouseButtonPressed: return "MouseButtonPressed";
    case sf::Event::MouseButtonReleased: return "MouseButtonReleased";
    case sf::Event::MouseMoved: return "MouseMoved";
    case sf::Event::Closed: return "WindowClosed";
    default: return "Other";
    }
}

std::string SettingsEventLogger::keyCodeToString(sf::Keyboard::Key key) const {
    switch (key) {
    case sf::Keyboard::Escape: return "Escape";
    case sf::Keyboard::Enter: return "Enter";
    case sf::Keyboard::U: return "U";
    case sf::Keyboard::R: return "R";
    case sf::Keyboard::H: return "H";
    case sf::Keyboard::LControl: return "LControl";
    case sf::Keyboard::RControl: return "RControl";
    default: return "Key" + std::to_string(static_cast<int>(key));
    }
}

void SettingsEventLogger::printToConsole(LogLevel level, const std::string& category, const std::string& message) {
    std::cout << "[" << logLevelToString(level) << "][" << category << "] " << message << std::endl;
}

void SettingsEventLogger::printToFile(LogLevel level, const std::string& category, const std::string& message) {
    if (m_logFile && m_logFile->is_open()) {
        *m_logFile << getCurrentTimestamp() << " [" << logLevelToString(level)
            << "][" << category << "] " << message << std::endl;
        m_logFile->flush(); // Ensure immediate write
    }
}