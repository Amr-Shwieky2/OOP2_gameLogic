#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Scripting/ScriptManager.h"
#include "Scripting/IScriptEngine.h"
#include "Scripting/LuaScriptEngine.h"
#include "mocks/MockScriptEngine.h"
#include <filesystem>
#include <fstream>

/**
 * @brief Test fixture for scripting system tests
 * 
 * Provides a common setup and teardown for scripting tests
 * with script engine initialization and test script files.
 */
class ScriptingFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test script directories
        createTestScriptDirectories();
        
        // Create mock script engine
        mockEngine = std::make_unique<Scripting::MockScriptEngine>();
    }
    
    void TearDown() override {
        mockEngine.reset();
    }
    
    // Create test script directories and ensure they exist
    void createTestScriptDirectories() {
        std::filesystem::create_directories("test_scripts");
    }
    
    // Helper method to create a test script file
    void createTestScript(const std::string& filename, const std::string& content) {
        std::ofstream scriptFile("test_scripts/" + filename);
        if (scriptFile) {
            scriptFile << content;
        }
    }
    
    // Mock script engine for testing
    std::unique_ptr<Scripting::MockScriptEngine> mockEngine;
};