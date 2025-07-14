#pragma once

#include <string>

enum class LogLevel {
    Info,
    Warning,
    Error
};

// Global lightweight logging utility
class Logger {
public:
    static void log(const std::string& message, LogLevel level = LogLevel::Info);

private:
    static void print(const std::string& prefix, const std::string& message);
};
