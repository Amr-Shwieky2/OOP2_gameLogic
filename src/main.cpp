#include "../include/Application/App.h"
#include "Logger.h"
#include <Services/Logger.h>

int main() {
    App app;
    try {
        app.run();
    }
    catch (const std::exception& e) {
        Logger::log("Unhandled exception: " + std::string(e.what()), LogLevel::Error);
        return -1;
    }
    catch (...) {
        Logger::log("Unknown error occurred", LogLevel::Error);
        return -1;
    }
    return 0;
}
