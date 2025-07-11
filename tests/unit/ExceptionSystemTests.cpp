#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Exceptions/GameExceptions.h"
#include "Exceptions/Logger.h"
#include "Exceptions/ExceptionRecoverySystem.h"
#include "Exceptions/GameCheckpoint.h"
#include "Exceptions/RecoveryStrategyFactory.h"
#include "Exceptions/UserFriendlyErrors.h"
#include <memory>

// Mock for user interface
class MockErrorDialog {
public:
    MOCK_METHOD(void, showError, (const GameExceptions::Exception&));
    MOCK_METHOD(void, showMessage, (const std::string&, const std::string&));
    MOCK_METHOD(bool, showConfirmation, (const std::string&, const std::string&));
};

// Test fixture for exception system
class ExceptionSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up logger
        auto& logger = GameExceptions::getLogger();
        logger.setLogLevel(GameExceptions::LogLevel::Debug);
        
        // Set up recovery system
        recoverySystem = &ExceptionRecoverySystem::getInstance();
        recoverySystem->initialize();
        
        // Set up mock dialog
        mockDialog = std::make_unique<MockErrorDialog>();
        
        // Set up game session for checkpoints
        gameSession = std::make_unique<GameSession>();
    }
    
    void TearDown() override {
        // Clean up
        recoverySystem->shutdown();
    }
    
    ExceptionRecoverySystem* recoverySystem = nullptr;
    std::unique_ptr<MockErrorDialog> mockDialog;
    std::unique_ptr<GameSession> gameSession;
};

// Test exception creation and properties
TEST_F(ExceptionSystemTest, ExceptionCreation) {
    // Create exception
    GameExceptions::Exception ex("Test error message", 
                                GameExceptions::Exception::Severity::Error,
                                123);
                                
    // Check properties
    EXPECT_EQ("Test error message", ex.what());
    EXPECT_EQ(GameExceptions::Exception::Severity::Error, ex.getSeverity());
    EXPECT_EQ(123, ex.getErrorCode());
    
    // Create derived exception
    GameExceptions::ResourceNotFoundException notFoundEx("missing.png");
    
    // Check properties
    EXPECT_EQ("Resource not found: missing.png", notFoundEx.what());
    EXPECT_EQ(GameExceptions::Exception::Severity::Error, notFoundEx.getSeverity());
    EXPECT_EQ(GameExceptions::ErrorCodes::RESOURCE_NOT_FOUND, notFoundEx.getErrorCode());
}

// Test logger functionality
TEST_F(ExceptionSystemTest, LoggerTest) {
    // Set up logger
    auto& logger = GameExceptions::getLogger();
    
    // Create a temporary log file
    const std::string testLogFile = "logs/test_exceptions.log";
    logger.setLogFile(testLogFile);
    
    // Log messages at different levels
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    logger.critical("Critical message");
    
    // Log an exception
    GameExceptions::Exception ex("Test exception", 
                                GameExceptions::Exception::Severity::Error,
                                456);
    logger.logException(ex, GameExceptions::LogLevel::Error);
    
    // Close log file to ensure it's flushed
    logger.setLogFile("");
    
    // Check that log file exists
    std::ifstream logFile(testLogFile);
    EXPECT_TRUE(logFile.good());
    
    // Check file contents (simple check)
    std::string content;
    std::string line;
    while (std::getline(logFile, line)) {
        content += line + "\n";
    }
    
    // Verify each level of message exists in the log
    EXPECT_TRUE(content.find("DEBUG") != std::string::npos);
    EXPECT_TRUE(content.find("INFO") != std::string::npos);
    EXPECT_TRUE(content.find("WARNING") != std::string::npos);
    EXPECT_TRUE(content.find("ERROR") != std::string::npos);
    EXPECT_TRUE(content.find("CRITICAL") != std::string::npos);
    
    // Check for exception details
    EXPECT_TRUE(content.find("Test exception") != std::string::npos);
    EXPECT_TRUE(content.find("456") != std::string::npos);
    
    // Clean up
    logFile.close();
}

// Test recovery strategies
TEST_F(ExceptionSystemTest, RecoveryStrategies) {
    // Create recovery strategies
    auto retryStrategy = RecoveryStrategyFactory::createRetryStrategy(
        [](const GameExceptions::ExceptionInfo& info) -> bool {
            return true; // Always succeed
        }
    );
    
    auto fallbackStrategy = RecoveryStrategyFactory::createFallbackStrategy(
        [](const GameExceptions::ExceptionInfo& info) -> bool {
            return true; // Always succeed
        }
    );
    
    auto checkpointStrategy = RecoveryStrategyFactory::createCheckpointStrategy(
        [](const GameExceptions::ExceptionInfo& info) -> std::shared_ptr<GameCheckpoint> {
            return std::make_shared<GameCheckpoint>("TestCheckpoint", nullptr);
        }
    );
    
    // Create test exception
    GameExceptions::Exception ex("Test exception", 
                                GameExceptions::Exception::Severity::Error,
                                789);
    GameExceptions::ExceptionInfo info(ex);
    info.component = "TestComponent";
    info.function = "testFunction";
    
    // Test retry strategy
    auto result = retryStrategy->apply(info);
    EXPECT_EQ(RecoveryResult::Success, result);
    
    // Test fallback strategy
    result = fallbackStrategy->apply(info);
    EXPECT_EQ(RecoveryResult::Success, result);
    
    // Test checkpoint strategy
    result = checkpointStrategy->apply(info);
    EXPECT_EQ(RecoveryResult::Success, result);
}

// Test exception recovery system
TEST_F(ExceptionSystemTest, ExceptionRecoverySystem) {
    // Register a strategy for a specific error code
    recoverySystem->registerStrategy(789, RecoveryStrategyFactory::createRetryStrategy(
        [](const GameExceptions::ExceptionInfo& info) -> bool {
            return true; // Always succeed
        }
    ));
    
    // Create exception with registered error code
    GameExceptions::Exception ex("Test exception", 
                                GameExceptions::Exception::Severity::Error,
                                789);
                                
    // Handle exception
    auto result = recoverySystem->handleException(ex);
    
    // Should be handled successfully
    EXPECT_EQ(RecoveryResult::Success, result);
    
    // Create exception with unregistered error code
    GameExceptions::Exception ex2("Unhandled exception", 
                                 GameExceptions::Exception::Severity::Error,
                                 999);
                                 
    // Handle exception
    result = recoverySystem->handleException(ex2);
    
    // Should not be handled
    EXPECT_EQ(RecoveryResult::Unhandled, result);
}

// Test checkpoint system
TEST_F(ExceptionSystemTest, CheckpointSystem) {
    // Create a checkpoint
    auto checkpoint = GameCheckpoint::create("TestCheckpoint", gameSession.get());
    
    // Verify checkpoint was created
    ASSERT_NE(nullptr, checkpoint);
    EXPECT_EQ("TestCheckpoint", checkpoint->getName());
    
    // Test checkpoint restore
    bool restoreResult = checkpoint->restore();
    
    // Basic checkpoint should restore successfully
    EXPECT_TRUE(restoreResult);
}

// Test user-friendly errors
TEST_F(ExceptionSystemTest, UserFriendlyErrors) {
    // Set up mock dialog
    ErrorDialog::setImplementation(mockDialog.get());
    
    // Expect error dialog to be shown
    EXPECT_CALL(*mockDialog, showError(::testing::_))
        .Times(1);
    
    // Show error
    GameExceptions::Exception ex("Test exception", 
                                GameExceptions::Exception::Severity::Error,
                                123);
    ErrorDialog::getInstance().showError(ex);
    
    // Expect message dialog to be shown
    EXPECT_CALL(*mockDialog, showMessage("Test Title", "Test Message"))
        .Times(1);
    
    // Show message
    ErrorDialog::getInstance().showMessage("Test Title", "Test Message");
    
    // Expect confirmation dialog to be shown
    EXPECT_CALL(*mockDialog, showConfirmation("Confirm Title", "Confirm Message"))
        .WillOnce(::testing::Return(true));
    
    // Show confirmation
    bool result = ErrorDialog::getInstance().showConfirmation("Confirm Title", "Confirm Message");
    EXPECT_TRUE(result);
}

// Test exception telemetry
TEST_F(ExceptionSystemTest, ExceptionTelemetry) {
    auto& telemetry = recoverySystem->getTelemetry();
    
    // Record some exceptions
    GameExceptions::Exception ex1("Error 1", 
                                 GameExceptions::Exception::Severity::Warning,
                                 101);
    GameExceptions::ExceptionInfo info1(ex1);
    info1.component = "Component1";
    telemetry.recordException(info1);
    
    GameExceptions::Exception ex2("Error 2", 
                                 GameExceptions::Exception::Severity::Error,
                                 102);
    GameExceptions::ExceptionInfo info2(ex2);
    info2.component = "Component2";
    telemetry.recordException(info2);
    
    GameExceptions::Exception ex3("Error 1", // Same as first error
                                 GameExceptions::Exception::Severity::Warning,
                                 101);
    GameExceptions::ExceptionInfo info3(ex3);
    info3.component = "Component1";
    telemetry.recordException(info3);
    
    // Get statistics
    auto stats = telemetry.getStatistics();
    
    // Check exception counts
    EXPECT_EQ(3, stats.totalExceptions);
    EXPECT_EQ(2, stats.uniqueExceptionTypes);
    
    // Check component frequencies
    auto componentFreq = telemetry.getComponentFrequencies();
    EXPECT_EQ(2, componentFreq["Component1"]);
    EXPECT_EQ(1, componentFreq["Component2"]);
    
    // Check error code frequencies
    auto codeFreq = telemetry.getErrorCodeFrequencies();
    EXPECT_EQ(2, codeFreq[101]);
    EXPECT_EQ(1, codeFreq[102]);
    
    // Test report generation
    std::string report = telemetry.generateReport();
    
    // Report should contain key information
    EXPECT_TRUE(report.find("Component1") != std::string::npos);
    EXPECT_TRUE(report.find("Component2") != std::string::npos);
    EXPECT_TRUE(report.find("101") != std::string::npos);
    EXPECT_TRUE(report.find("102") != std::string::npos);
}