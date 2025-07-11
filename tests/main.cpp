#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <filesystem>
#include "Exceptions/Logger.h"
#include "TestReportGenerator.h"

/**
 * Test environment for global setup and teardown
 */
class GameTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        // Global setup for all tests
        std::cout << "Setting up test environment..." << std::endl;
        
        // Create logs directory for tests
        try {
            std::filesystem::path logDir = "logs";
            if (!std::filesystem::exists(logDir)) {
                std::filesystem::create_directory(logDir);
            }
            
            // Create reports directory
            std::filesystem::path reportsDir = "test_reports";
            if (!std::filesystem::exists(reportsDir)) {
                std::filesystem::create_directory(reportsDir);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create directories: " << e.what() << std::endl;
        }
        
        // Initialize logger for tests
        GameExceptions::getLogger().setLogLevel(GameExceptions::LogLevel::Debug);
        GameExceptions::getLogger().info("Test environment initialized");
    }
    
    void TearDown() override {
        // Global cleanup for all tests
        std::cout << "Tearing down test environment..." << std::endl;
        GameExceptions::getLogger().info("Test environment cleanup completed");
    }
};

/**
 * Main function for all tests
 */
int main(int argc, char** argv) {
    // Parse command line arguments
    bool generateReports = true;
    std::string reportOutputDir = "test_reports";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--no-reports") {
            generateReports = false;
        } else if (arg == "--report-dir" && i + 1 < argc) {
            reportOutputDir = argv[++i];
        }
    }
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Add the global test environment
    ::testing::AddGlobalTestEnvironment(new GameTestEnvironment);
    
    // Enable test reporting if requested
    if (generateReports) {
        EnableTestReporting(reportOutputDir);
    }
    
    std::cout << "Running tests..." << std::endl;
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    return result;
}