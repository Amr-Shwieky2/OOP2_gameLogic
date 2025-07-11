#include "App.h"
#include <iostream>

// A simple main file that just calls the main app
int main(int argc, char* argv[]) {
    try {
        App app;
        return app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
