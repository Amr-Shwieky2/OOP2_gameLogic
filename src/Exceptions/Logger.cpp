#include "Exceptions/Logger.h"
#include "Exceptions/Exception.h"
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <ctime>

namespace GameExceptions {

//----------------------------------------------------------------------------------------
// ConsoleLogTarget implementation
//----------------------------------------------------------------------------------------

void ConsoleLogTarget::write(LogLevel level, const std::string& message) {
    // Output to appropriate stream based on level
    std::ostream& stream = (level >= LogLevel::Error) ? std::cerr : std::cout;
    
    // Simple output with no coloring
    stream << message << std::endl;
}

void ConsoleLogTarget::flush() {
    std::cout.flush();
    std::cerr.flush();
}

//----------------------------------------------------------------------------------------
// FileLogTarget implementation
//----------------------------------------------------------------------------------------

FileLogTarget::FileLogTarget(const std::string& filename) {
    // Create directory if it doesn't exist
    std::filesystem::path logDir = "logs";
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directory(logDir);
    }
    
    // Open file with timestamp in name if specified
    std::string actualFilename = filename;
    if (filename == "auto") {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::tm tm_now;
#ifdef _MSC_VER
        localtime_s(&tm_now, &time_t_now);
#else
        tm_now = *std::localtime(&time_t_now);
#endif

        std::ostringstream oss;
        oss << "logs/game_log_" 
            << std::put_time(&tm_now, "%Y%m%d_%H%M%S") 
            << ".log";
        actualFilename = oss.str();
    }
    else {
        actualFilename = "logs/" + filename;
    }
    
    m_file.open(actualFilename, std::ios::out | std::ios::app);
    if (!m_file.is_open()) {
        throw std::runtime_error("Failed to open log file: " + actualFilename);
    }
}

FileLogTarget::~FileLogTarget() {
    if (m_file.is_open()) {
        m_file.close();
    }
}

void FileLogTarget::write(LogLevel, const std::string& message) {
    if (m_file.is_open()) {
        m_file << message << std::endl;
    }
}

void FileLogTarget::flush() {
    if (m_file.is_open()) {
        m_file.flush();
    }
}

//----------------------------------------------------------------------------------------
// Logger implementation
//----------------------------------------------------------------------------------------

Logger::Logger() {
    // Default to console logging
    addTarget(std::make_unique<ConsoleLogTarget>());
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::addTarget(std::unique_ptr<LogTarget> target) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_targets.push_back(std::move(target));
}

void Logger::clearTargets() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_targets.clear();
}

void Logger::log(LogLevel level, const std::string& message, const std::source_location& location) {
    // Skip if below minimum level
    if (level < m_minLevel) {
        return;
    }
    
    std::string formattedMessage = formatLogMessage(level, message, location);
    
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& target : m_targets) {
        target->write(level, formattedMessage);
    }
}

void Logger::debug(const std::string& message, const std::source_location& location) {
    log(LogLevel::Debug, message, location);
}

void Logger::info(const std::string& message, const std::source_location& location) {
    log(LogLevel::Info, message, location);
}

void Logger::warning(const std::string& message, const std::source_location& location) {
    log(LogLevel::Warning, message, location);
}

void Logger::error(const std::string& message, const std::source_location& location) {
    log(LogLevel::Error, message, location);
}

void Logger::critical(const std::string& message, const std::source_location& location) {
    log(LogLevel::Critical, message, location);
}

void Logger::logException(const std::exception& ex, LogLevel level, const std::source_location& location) {
    // Handle our custom exceptions specially
    if (auto gameEx = dynamic_cast<const Exception*>(&ex)) {
        // Map severity to log level if not specified
        if (level == LogLevel::Error) { // If using the default level
            switch (gameEx->getSeverity()) {
                case Exception::Severity::Info:
                    level = LogLevel::Info;
                    break;
                case Exception::Severity::Warning:
                    level = LogLevel::Warning;
                    break;
                case Exception::Severity::Error:
                    level = LogLevel::Error;
                    break;
                case Exception::Severity::Critical:
                case Exception::Severity::Fatal:
                    level = LogLevel::Critical;
                    break;
            }
        }
        
        // Use the exception's formatted message directly
        log(level, gameEx->what(), location);
    }
    else {
        // Handle standard exceptions
        log(level, std::format("Exception: {}", ex.what()), location);
    }
}

std::string Logger::formatLogMessage(LogLevel level, const std::string& message, const std::source_location& location) {
    auto now = std::chrono::system_clock::now();
    std::string timestamp = formatTimestamp(now);
    
    // Get only the filename from the full path
    std::string filename = std::filesystem::path(location.file_name()).filename().string();
    
    return std::format("[{}] {} [{}:{}] [{}] {}", 
                       logLevelToString(level),
                       timestamp,
                       filename,
                       location.line(),
                       location.function_name(),
                       message);
}

std::string Logger::formatTimestamp(const std::chrono::system_clock::time_point& tp) {
    auto time_t_now = std::chrono::system_clock::to_time_t(tp);
    
    std::tm tm_now;
#ifdef _MSC_VER
    localtime_s(&tm_now, &time_t_now);
#else
    tm_now = *std::localtime(&time_t_now);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");
    
    // Add milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch() % std::chrono::seconds(1));
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARNING";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

} // namespace GameExceptions