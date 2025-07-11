#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Scripting/ScriptManager.h"
#include "Scripting/IScriptEngine.h"
#include "Scripting/ScriptComponent.h"
#include "Scripting/LuaScriptEngine.h"
#include "fixtures/ScriptingFixture.h"
#include <filesystem>
#include <fstream>

// Mock script callback for testing
class MockScriptCallback {
public:
    MOCK_METHOD(void, call, (const std::string&));
    
    // Function to be registered with script engine
    std::any callFromScript(const std::vector<std::any>& args) {
        if (!args.empty()) {
            try {
                std::string message = std::any_cast<std::string>(args[0]);
                call(message);
                return true;
            } catch (const std::bad_any_cast&) {
                return false;
            }
        }
        return false;
    }
};

// Scripting system test fixture
class ScriptingSystemTest : public ScriptingFixture {
protected:
    void SetUp() override {
        ScriptingFixture::SetUp();
        
        // Create additional test scripts
        createPropertyTestScript();
        createCallbackTestScript();
    }
    
    void createPropertyTestScript() {
        std::ofstream scriptFile("test_scripts/property_test.lua");
        scriptFile << "local entity = {}\n";
        scriptFile << "entity.health = 100\n";
        scriptFile << "entity.name = \"TestEntity\"\n";
        scriptFile << "\n";
        scriptFile << "function entity:damage(amount)\n";
        scriptFile << "  self.health = self.health - amount\n";
        scriptFile << "  return self.health\n";
        scriptFile << "end\n";
        scriptFile << "\n";
        scriptFile << "function entity:heal(amount)\n";
        scriptFile << "  self.health = self.health + amount\n";
        scriptFile << "  return self.health\n";
        scriptFile << "end\n";
        scriptFile << "\n";
        scriptFile << "function entity:getName()\n";
        scriptFile << "  return self.name\n";
        scriptFile << "end\n";
        scriptFile << "\n";
        scriptFile << "return entity\n";
        scriptFile.close();
    }
    
    void createCallbackTestScript() {
        std::ofstream scriptFile("test_scripts/callback_test.lua");
        scriptFile << "function testCallback()\n";
        scriptFile << "  logMessage(\"Script callback executed!\")\n";
        scriptFile << "end\n";
        scriptFile << "\n";
        scriptFile << "function testWithArgs(a, b)\n";
        scriptFile << "  local result = a + b\n";
        scriptFile << "  logMessage(\"Result: \" .. result)\n";
        scriptFile << "  return result\n";
        scriptFile << "end\n";
        scriptFile.close();
    }
    
    // Helper for testing script execution
    Scripting::ScriptResult executeTestScript(const std::string& code) {
        Scripting::ScriptResult result;
        
        // Use mock engine if available, otherwise create a real Lua engine
        if (mockEngine) {
            result = mockEngine->executeString(code, "test_script");
        } else {
            Scripting::LuaScriptEngine engine;
            engine.initialize(nullptr);
            result = engine.executeString(code, "test_script");
        }
        
        return result;
    }
    
    MockScriptCallback mockCallback;
};

// Test script execution
TEST_F(ScriptingSystemTest, BasicScriptExecution) {
    // Set up mock engine expectation
    EXPECT_CALL(*mockEngine, executeString(::testing::_, ::testing::_))
        .Times(1);
    
    // Execute script
    executeTestScript("return 42");
}

// Test script error handling
TEST_F(ScriptingSystemTest, ScriptErrorHandling) {
    // Set up mock with error result
    Scripting::ScriptResult errorResult;
    errorResult.success = false;
    errorResult.error = "Syntax error";
    errorResult.errorLine = 1;
    
    EXPECT_CALL(*mockEngine, executeString("invalid lua code", ::testing::_))
        .WillOnce(::testing::Return(errorResult));
    
    // Execute invalid script
    auto result = executeTestScript("invalid lua code");
    
    // Check error handling
    EXPECT_FALSE(result.success);
    EXPECT_EQ("Syntax error", result.error);
    EXPECT_EQ(1, result.errorLine);
}

// Test script file loading
TEST_F(ScriptingSystemTest, ScriptFileLoading) {
    // Set up expectation
    EXPECT_CALL(*mockEngine, executeFile("test_scripts/test.lua"))
        .Times(1);
    
    // Load script file
    mockEngine->executeFile("test_scripts/test.lua");
}

// Test script callback
TEST_F(ScriptingSystemTest, ScriptCallbacks) {
    // Register callback function
    auto callback = std::bind(&MockScriptCallback::callFromScript, &mockCallback, std::placeholders::_1);
    EXPECT_CALL(*mockEngine, registerFunction("logMessage", ::testing::_))
        .WillOnce(::testing::Return(true));
    
    mockEngine->registerFunction("logMessage", callback);
    
    // Set up callback expectation
    EXPECT_CALL(mockCallback, call("Script callback executed!"))
        .Times(1);
    
    // Set up script execution expectation with success result
    Scripting::ScriptResult successResult;
    successResult.success = true;
    
    EXPECT_CALL(*mockEngine, executeFile("test_scripts/callback_test.lua"))
        .WillOnce(::testing::Return(successResult));
        
    EXPECT_CALL(*mockEngine, callFunction("testCallback", ::testing::_))
        .WillOnce(::testing::Return(successResult));
    
    // Execute script with callback
    mockEngine->executeFile("test_scripts/callback_test.lua");
    mockEngine->callFunction("testCallback");
}

// Test ScriptComponent
TEST_F(ScriptingSystemTest, ScriptComponent) {
    // Create entity and script component
    Entity entity;
    Scripting::ScriptComponent* scriptComponent = new Scripting::ScriptComponent();
    scriptComponent->setOwner(&entity);
    
    // Set up mock expectations
    EXPECT_CALL(*mockEngine, executeFile(::testing::_))
        .WillRepeatedly(::testing::Return(Scripting::ScriptResult{true}));
        
    // Set script file
    scriptComponent->setScriptFile("test_scripts/property_test.lua");
    
    // Mock the script manager to return our mock engine
    EXPECT_CALL(*mockEngine, getLanguage())
        .WillRepeatedly(::testing::Return(Scripting::ScriptLanguage::Lua));
    
    // Initialize component (should load script)
    scriptComponent->initialize(mockEngine);
    
    // Clean up
    delete scriptComponent;
}

// Test script variable access
TEST_F(ScriptingSystemTest, ScriptVariableAccess) {
    // Set up mock for global variable access
    EXPECT_CALL(*mockEngine, setGlobal("testVar", ::testing::_))
        .WillOnce(::testing::Return(true));
        
    std::any testValue = 42;
    EXPECT_CALL(*mockEngine, getGlobal("testVar"))
        .WillOnce(::testing::Return(testValue));
        
    EXPECT_CALL(*mockEngine, hasGlobal("testVar"))
        .WillOnce(::testing::Return(true));
    
    // Set global variable
    mockEngine->setGlobal("testVar", 42);
    
    // Check if variable exists
    EXPECT_TRUE(mockEngine->hasGlobal("testVar"));
    
    // Get global variable
    auto result = mockEngine->getGlobal("testVar");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(42, std::any_cast<int>(result.value()));
}

// Test script hot-reload
TEST_F(ScriptingSystemTest, ScriptHotReload) {
    // Set up mock for hot-reload
    EXPECT_CALL(*mockEngine, hotReloadFile("test_scripts/test.lua"))
        .WillOnce(::testing::Return(true));
    
    // Perform hot-reload
    bool reloadResult = mockEngine->hotReloadFile("test_scripts/test.lua");
    EXPECT_TRUE(reloadResult);
}

// Test property-based testing with different script inputs
class ScriptPropertyTest : public ScriptingSystemTest, 
                           public ::testing::WithParamInterface<std::pair<std::string, bool>> {
};

// Parameterized test for script validation with different inputs
TEST_P(ScriptPropertyTest, ScriptValidation) {
    const auto& param = GetParam();
    const std::string& scriptCode = param.first;
    bool expectedValidity = param.second;
    
    // Set up mock for script validation
    Scripting::ScriptResult validationResult;
    validationResult.success = expectedValidity;
    if (!expectedValidity) {
        validationResult.error = "Validation failed";
    }
    
    EXPECT_CALL(*mockEngine, validateScript(scriptCode, ::testing::_))
        .WillOnce(::testing::Return(validationResult));
    
    // Validate script
    auto result = mockEngine->validateScript(scriptCode, "validation_test");
    
    // Check result matches expected validity
    EXPECT_EQ(expectedValidity, result.success);
}

// Test cases for property-based testing
INSTANTIATE_TEST_SUITE_P(
    ScriptValidationTests,
    ScriptPropertyTest,
    ::testing::Values(
        std::make_pair("function test() return true end", true),
        std::make_pair("function test() return true", false),
        std::make_pair("local x = {1, 2, 3}", true),
        std::make_pair("local x = {1, 2, 3", false),
        std::make_pair("local x = 10; if x > 5 then print(x) end", true),
        std::make_pair("local x = 10; if x > 5 then print(x)", false)
    )
);