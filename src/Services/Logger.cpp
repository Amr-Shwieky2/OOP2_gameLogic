#include <iostream>
#include <Services/Logger.h>


void Logger::log(const std::string& message, LogLevel level) {
    switch (level) {
    case LogLevel::Info:
        print("[INFO] ", message);
        break;
    case LogLevel::Warning:
        print("[WARN] ", message);
        break;
    case LogLevel::Error:
        print("[ERROR]", message);
        break;
    }
}

void Logger::print(const std::string& prefix, const std::string& message) {
    std::cout << prefix << " " << message << std::endl;
}
