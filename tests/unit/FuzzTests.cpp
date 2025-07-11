#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Exceptions/GameExceptions.h"
#include "Scripting/ScriptValidator.h"
#include "Scripting/LuaScriptEngine.h"
#include "MultiMethodDispatcher.h"
#include "Exceptions/ExceptionRecoverySystem.h"
#include "Memory/MemoryManager.h"
#include "Memory/MemoryLeakDetector.h"
#include <random>
#include <string>
#include <vector>
#include <memory>

/**
 * @brief Fuzz testing framework for input validation and error handling
 * 
 * This class provides utilities for generating random/malformed inputs
 * to test system robustness and error handling.
 */
class FuzzTestFramework {
public:
    // Random number generator
    static std::mt19937& getRNG() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
    
    // Generate random string of specified length
    static std::string randomString(size_t length) {
        static const char charset[] = 
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "!@#$%^&*()_+=-{}[]|:;\"'<>,.?/";
            
        std::string result;
        result.resize(length);
        
        std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);
        
        for (size_t i = 0; i < length; ++i) {
            result[i] = charset[dist(getRNG())];
        }
        
        return result;
    }
    
    // Generate random number in range [min, max]
    template<typename T>
    static T randomNumber(T min, T max) {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(getRNG());
    }
    
    // Generate random boolean with given probability of being true
    static bool randomBool(double trueProbability = 0.5) {
        std::bernoulli_distribution dist(trueProbability);
        return dist(getRNG());
    }
    
    // Generate malformed Lua script
    static std::string generateMalformedLuaScript() {
        // Start with valid Lua structure
        std::string script = "function test()\n";
        
        // Number of lines to generate
        int lines = randomNumber(1, 10);
        
        for (int i = 0; i < lines; ++i) {
            // Randomly decide what to add
            int option = randomNumber(0, 4);
            
            switch (option) {
                case 0: // Valid statement
                    script += "    local x = " + std::to_string(randomNumber(1, 100)) + "\n";
                    break;
                case 1: // Unbalanced parentheses
                    script += "    print(\"test\"" + std::string(randomBool() ? "" : ")") + "\n";
                    break;
                case 2: // Invalid syntax
                    script += "    if x > " + std::to_string(randomNumber(1, 10)) + " then\n";
                    // Sometimes omit the 'end'
                    if (randomBool(0.7)) {
                        script += "    end\n";
                    }
                    break;
                case 3: // Reference undefined variable
                    script += "    print(" + randomString(5) + ")\n";
                    break;
                case 4: // Random invalid code
                    script += "    " + randomString(randomNumber(5, 20)) + "\n";
                    break;
            }
        }
        
        // End the function (sometimes)
        if (randomBool(0.7)) {
            script += "end\n";
        }
        
        return script;
    }
    
    // Generate random entity position
    static sf::Vector2f randomPosition() {
        return sf::Vector2f(
            randomNumber(-1000.0f, 1000.0f),
            randomNumber(-1000.0f, 1000.0f)
        );
    }
    
    // Generate random direction vector
    static sf::Vector2f randomDirection() {
        float angle = randomNumber(0.0f, 2.0f * 3.14159f);
        return sf::Vector2f(std::cos(angle), std::sin(angle));
    }
};

/**
 * @brief Test fixture for fuzz testing
 */
class FuzzTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up exception recovery system
        recoverySystem = &ExceptionRecoverySystem::getInstance();
        
        // Enable memory tracking
        MemoryLeakDetector::getInstance().setEnabled(true);
        
        // Set up Lua engine
        scriptEngine = std::make_unique<Scripting::LuaScriptEngine>();
        scriptEngine->initialize(nullptr);
        
        // Track initial state
        initialMemoryUsage = MemoryManager::getInstance().getTotalMemoryUsage();
    }
    
    void TearDown() override {
        // Clean up resources
        scriptEngine.reset();
        
        // Check for memory leaks
        size_t finalMemoryUsage = MemoryManager::getInstance().getTotalMemoryUsage();
        
        // We expect memory to stay the same
        EXPECT_EQ(initialMemoryUsage, finalMemoryUsage) 
            << "Memory leak detected: " 
            << (finalMemoryUsage - initialMemoryUsage) << " bytes";
    }
    
    ExceptionRecoverySystem* recoverySystem = nullptr;
    std::unique_ptr<Scripting::LuaScriptEngine> scriptEngine;
    size_t initialMemoryUsage = 0;
};

// Test script validation with fuzzed inputs
TEST_F(FuzzTest, FuzzScriptValidation) {
    // Number of fuzz iterations
    const int ITERATIONS = 100;
    
    // Counter for valid/invalid scripts
    int validCount = 0;
    int invalidCount = 0;
    
    // Fuzz test the script engine with random inputs
    for (int i = 0; i < ITERATIONS; ++i) {
        // Generate random script
        std::string script = FuzzTestFramework::generateMalformedLuaScript();
        
        try {
            // Attempt to validate the script
            auto result = scriptEngine->validateScript(script, "fuzz_test");
            
            if (result.success) {
                validCount++;
            } else {
                invalidCount++;
            }
            
            // Check that even with invalid scripts, we don't crash
            // and properly report the error
            if (!result.success) {
                EXPECT_FALSE(result.error.empty());
                EXPECT_GT(result.errorLine, 0);
            }
        }
        catch (const GameExceptions::Exception& ex) {
            // Script validation should not throw exceptions
            FAIL() << "Script validation threw an exception: " << ex.what();
        }
        catch (const std::exception& ex) {
            // Standard exception - fail test
            FAIL() << "Script validation threw a standard exception: " << ex.what();
        }
        catch (...) {
            // Any other exception - fail test
            FAIL() << "Script validation threw an unknown exception";
        }
    }
    
    // Log results
    std::cout << "Fuzz tested " << ITERATIONS << " scripts: "
              << validCount << " valid, " 
              << invalidCount << " invalid" << std::endl;
    
    // We expect most scripts to be invalid
    EXPECT_GT(invalidCount, validCount);
}

// Test MultiMethodDispatcher with random types
TEST_F(FuzzTest, FuzzMultiMethodDispatcher) {
    auto& dispatcher = MultiMethodDispatcher::getInstance();
    
    // Create random test objects
    class FuzzObject1 {
    public:
        explicit FuzzObject1(int id) : m_id(id) {}
        int getId() const { return m_id; }
    private:
        int m_id;
    };
    
    class FuzzObject2 {
    public:
        explicit FuzzObject2(int id) : m_id(id) {}
        int getId() const { return m_id; }
    private:
        int m_id;
    };
    
    // Register handler
    dispatcher.registerHandler<FuzzObject1, FuzzObject2>("fuzz_test",
        [](FuzzObject1& obj1, FuzzObject2& obj2) {
            // Just a test handler, nothing to do
        }
    );
    
    // Number of fuzz iterations
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        try {
            // Create random objects
            FuzzObject1 obj1(FuzzTestFramework::randomNumber(1, 1000));
            FuzzObject2 obj2(FuzzTestFramework::randomNumber(1, 1000));
            
            // Generate random method name
            std::string methodName = FuzzTestFramework::randomBool(0.5) ? 
                "fuzz_test" : // Valid method name
                FuzzTestFramework::randomString(10); // Invalid method name
            
            // Dispatch with method name
            dispatcher.dispatch(obj1, obj2, methodName);
            
            // No assertion needed - we're just verifying the system doesn't crash
        }
        catch (const std::exception& ex) {
            FAIL() << "Exception during MultiMethodDispatcher fuzz test: " << ex.what();
        }
    }
}

// Test exception recovery system with fuzzed exceptions
TEST_F(FuzzTest, FuzzExceptionRecovery) {
    // Number of fuzz iterations
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        try {
            // Generate random exception
            int severity = FuzzTestFramework::randomNumber(0, 2);
            int errorCode = FuzzTestFramework::randomNumber(100, 999);
            std::string errorMessage = FuzzTestFramework::randomString(
                FuzzTestFramework::randomNumber(10, 100));
            
            // Create exception
            GameExceptions::Exception ex(
                errorMessage,
                static_cast<GameExceptions::Exception::Severity>(severity),
                errorCode
            );
            
            // Create exception info
            GameExceptions::ExceptionInfo exInfo(ex);
            exInfo.component = FuzzTestFramework::randomString(10);
            exInfo.function = FuzzTestFramework::randomString(15);
            
            // Handle the exception
            auto result = recoverySystem->handleException(ex);
            
            // The system shouldn't crash, but we don't expect recovery to succeed
            // for random exceptions
            EXPECT_NE(RecoveryResult::Unhandled, result);
        }
        catch (const std::exception& ex) {
            FAIL() << "Exception during ExceptionRecoverySystem fuzz test: " << ex.what();
        }
    }
}

// Test memory allocation with fuzzed sizes
TEST_F(FuzzTest, FuzzMemoryAllocation) {
    auto& memoryManager = MemoryManager::getInstance();
    
    // Number of fuzz iterations
    const int ITERATIONS = 100;
    
    // Objects allocated
    std::vector<void*> allocations;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        try {
            // Generate random allocation size
            size_t size = FuzzTestFramework::randomNumber<size_t>(1, 10000);
            
            // Allocate memory
            void* ptr = memoryManager.allocate(size);
            
            // Should not return null
            ASSERT_NE(nullptr, ptr);
            
            // Store allocation
            allocations.push_back(ptr);
            
            // Randomly free some allocations
            if (FuzzTestFramework::randomBool(0.7) && !allocations.empty()) {
                size_t index = FuzzTestFramework::randomNumber<size_t>(0, allocations.size() - 1);
                void* toFree = allocations[index];
                
                // Erase from vector
                allocations.erase(allocations.begin() + index);
                
                // Free memory
                memoryManager.deallocate(toFree);
            }
        }
        catch (const std::exception& ex) {
            FAIL() << "Exception during MemoryManager fuzz test: " << ex.what();
        }
    }
    
    // Free remaining allocations
    for (void* ptr : allocations) {
        memoryManager.deallocate(ptr);
    }
}

// Parameterized fuzz tests for script input
class ScriptFuzzTest : public FuzzTest,
                       public ::testing::WithParamInterface<int> {
};

// Parameterized test for script execution with different script lengths
TEST_P(ScriptFuzzTest, FuzzScriptExecution) {
    const int scriptLength = GetParam();
    
    // Generate script of specified length
    std::string script = "function test()\n";
    for (int i = 0; i < scriptLength; ++i) {
        script += "    local x" + std::to_string(i) + " = " + 
                  std::to_string(FuzzTestFramework::randomNumber(1, 100)) + "\n";
    }
    script += "    return 42\n";
    script += "end\n";
    script += "return test()";
    
    try {
        // Execute the script
        auto result = scriptEngine->executeString(script, "fuzz_param_test");
        
        // Check execution result
        if (result.success && result.returnValue.has_value()) {
            // For valid scripts, we expect a return value of 42
            try {
                int value = std::any_cast<int>(result.returnValue.value());
                EXPECT_EQ(42, value);
            }
            catch (const std::bad_any_cast&) {
                FAIL() << "Script returned unexpected type";
            }
        }
    }
    catch (const std::exception& ex) {
        FAIL() << "Exception during script execution: " << ex.what();
    }
}

// Test with different script lengths
INSTANTIATE_TEST_SUITE_P(
    ScriptLengths,
    ScriptFuzzTest,
    ::testing::Values(1, 5, 10, 50, 100)
);