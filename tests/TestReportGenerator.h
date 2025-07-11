#pragma once
#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <filesystem>

/**
 * @brief Detailed test report generator
 * 
 * Creates HTML and XML test reports with coverage metrics, execution time,
 * and failure analysis.
 */
class TestReportGenerator : public ::testing::EmptyTestEventListener {
public:
    TestReportGenerator(const std::string& outputDir = "test_reports") 
        : m_outputDir(outputDir) {
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(outputDir);
        
        // Start timer for overall test duration
        m_startTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "Test reporting initialized. Reports will be saved to: " 
                 << outputDir << std::endl;
    }
    
    ~TestReportGenerator() {
        // Generate final reports when object is destroyed
        generateReports();
    }
    
    // Called before test suite starts
    void OnTestSuiteStart(const ::testing::TestSuite& test_suite) override {
        // Record suite start time
        m_suiteStartTimes[test_suite.name()] = std::chrono::high_resolution_clock::now();
    }
    
    // Called after test suite ends
    void OnTestSuiteEnd(const ::testing::TestSuite& test_suite) override {
        // Calculate duration
        auto endTime = std::chrono::high_resolution_clock::now();
        auto startTime = m_suiteStartTimes[test_suite.name()];
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Store suite results
        TestSuiteResult result;
        result.name = test_suite.name();
        result.testCount = test_suite.total_test_count();
        result.failedCount = test_suite.failed_test_count();
        result.disabledCount = test_suite.disabled_test_count();
        result.durationMs = duration.count();
        
        m_suiteResults.push_back(result);
    }
    
    // Called before a test starts
    void OnTestStart(const ::testing::TestInfo& test_info) override {
        // Record test start time
        m_testStartTimes[getFullTestName(test_info)] = std::chrono::high_resolution_clock::now();
    }
    
    // Called after a test ends
    void OnTestEnd(const ::testing::TestInfo& test_info) override {
        // Calculate duration
        auto endTime = std::chrono::high_resolution_clock::now();
        auto fullName = getFullTestName(test_info);
        auto startTime = m_testStartTimes[fullName];
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Store test result
        TestResult result;
        result.suiteName = test_info.test_suite_name();
        result.testName = test_info.name();
        result.fullName = fullName;
        result.durationMs = duration.count();
        result.status = test_info.result()->Passed() ? "PASSED" :
                        test_info.result()->Failed() ? "FAILED" : "SKIPPED";
        
        // Store test output if it failed
        if (test_info.result()->Failed()) {
            // Get test failure message
            const ::testing::TestResult* testResult = test_info.result();
            if (testResult->total_part_count() > 0) {
                for (int i = 0; i < testResult->total_part_count(); ++i) {
                    const ::testing::TestPartResult& part = testResult->GetTestPartResult(i);
                    if (part.failed()) {
                        result.failureMessage += part.message();
                        result.failureMessage += "\n";
                    }
                }
            }
        }
        
        m_testResults.push_back(result);
    }
    
private:
    // Test suite result structure
    struct TestSuiteResult {
        std::string name;
        int testCount = 0;
        int failedCount = 0;
        int disabledCount = 0;
        int64_t durationMs = 0;
    };
    
    // Test result structure
    struct TestResult {
        std::string suiteName;
        std::string testName;
        std::string fullName;
        std::string status;
        int64_t durationMs = 0;
        std::string failureMessage;
    };
    
    // Helper to get full test name
    std::string getFullTestName(const ::testing::TestInfo& test_info) {
        return std::string(test_info.test_suite_name()) + "." + std::string(test_info.name());
    }
    
    // Generate all reports
    void generateReports() {
        // Calculate total duration
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime);
        
        // Generate different report formats
        generateHTMLReport(totalDuration.count());
        generateXMLReport(totalDuration.count());
        generateCSVReport(totalDuration.count());
        
        // Print summary to console
        printSummary(totalDuration.count());
    }
    
    // Generate HTML report
    void generateHTMLReport(int64_t totalDurationMs) {
        std::ofstream html(m_outputDir + "/report.html");
        
        // Count total tests and failures
        int totalTests = 0;
        int totalFailures = 0;
        for (const auto& suite : m_suiteResults) {
            totalTests += suite.testCount;
            totalFailures += suite.failedCount;
        }
        
        // HTML header
        html << "<!DOCTYPE html>\n"
             << "<html>\n"
             << "<head>\n"
             << "  <title>Test Report</title>\n"
             << "  <style>\n"
             << "    body { font-family: Arial, sans-serif; margin: 20px; }\n"
             << "    h1, h2 { color: #333; }\n"
             << "    .summary { background-color: #f8f9fa; padding: 15px; border-radius: 5px; margin-bottom: 20px; }\n"
             << "    .passed { color: green; }\n"
             << "    .failed { color: red; }\n"
             << "    table { border-collapse: collapse; width: 100%; margin-top: 10px; }\n"
             << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
             << "    th { background-color: #f2f2f2; }\n"
             << "    tr:nth-child(even) { background-color: #f9f9f9; }\n"
             << "    .failure-message { background-color: #fff0f0; border-left: 3px solid #ff0000; padding: 10px; margin-top: 5px; }\n"
             << "  </style>\n"
             << "</head>\n"
             << "<body>\n"
             << "  <h1>Test Report</h1>\n";
        
        // Summary section
        html << "  <div class=\"summary\">\n"
             << "    <h2>Summary</h2>\n"
             << "    <p>Total Test Suites: " << m_suiteResults.size() << "</p>\n"
             << "    <p>Total Tests: " << totalTests << "</p>\n"
             << "    <p>Passed: <span class=\"passed\">" << (totalTests - totalFailures) << "</span></p>\n"
             << "    <p>Failed: <span class=\"failed\">" << totalFailures << "</span></p>\n"
             << "    <p>Pass Rate: " << (totalTests > 0 ? (100.0 * (totalTests - totalFailures) / totalTests) : 0) << "%</p>\n"
             << "    <p>Total Duration: " << (totalDurationMs / 1000.0) << " seconds</p>\n"
             << "  </div>\n";
        
        // Test suites section
        html << "  <h2>Test Suites</h2>\n"
             << "  <table>\n"
             << "    <tr><th>Suite Name</th><th>Tests</th><th>Passed</th><th>Failed</th><th>Disabled</th><th>Duration (ms)</th></tr>\n";
        
        for (const auto& suite : m_suiteResults) {
            html << "    <tr>\n"
                 << "      <td>" << suite.name << "</td>\n"
                 << "      <td>" << suite.testCount << "</td>\n"
                 << "      <td>" << (suite.testCount - suite.failedCount - suite.disabledCount) << "</td>\n"
                 << "      <td>" << suite.failedCount << "</td>\n"
                 << "      <td>" << suite.disabledCount << "</td>\n"
                 << "      <td>" << suite.durationMs << "</td>\n"
                 << "    </tr>\n";
        }
        
        html << "  </table>\n";
        
        // Test details section
        html << "  <h2>Test Details</h2>\n"
             << "  <table>\n"
             << "    <tr><th>Suite</th><th>Test</th><th>Status</th><th>Duration (ms)</th></tr>\n";
        
        for (const auto& test : m_testResults) {
            html << "    <tr>\n"
                 << "      <td>" << test.suiteName << "</td>\n"
                 << "      <td>" << test.testName << "</td>\n"
                 << "      <td class=\"" << (test.status == "PASSED" ? "passed" : "failed") << "\">" 
                 << test.status << "</td>\n"
                 << "      <td>" << test.durationMs << "</td>\n"
                 << "    </tr>\n";
                 
            if (!test.failureMessage.empty()) {
                html << "    <tr>\n"
                     << "      <td colspan=\"4\">\n"
                     << "        <div class=\"failure-message\">\n"
                     << "          <pre>" << test.failureMessage << "</pre>\n"
                     << "        </div>\n"
                     << "      </td>\n"
                     << "    </tr>\n";
            }
        }
        
        html << "  </table>\n";
        
        // HTML footer
        html << "</body>\n"
             << "</html>\n";
    }
    
    // Generate XML report
    void generateXMLReport(int64_t totalDurationMs) {
        std::ofstream xml(m_outputDir + "/report.xml");
        
        // XML header
        xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<testsuites>\n";
        
        // Group tests by suite
        std::map<std::string, std::vector<TestResult>> testsBySuite;
        for (const auto& test : m_testResults) {
            testsBySuite[test.suiteName].push_back(test);
        }
        
        // Output test suites
        for (const auto& suite : m_suiteResults) {
            int failures = suite.failedCount;
            int disabled = suite.disabledCount;
            int tests = suite.testCount;
            
            xml << "  <testsuite name=\"" << suite.name
                << "\" tests=\"" << tests
                << "\" failures=\"" << failures
                << "\" disabled=\"" << disabled
                << "\" time=\"" << (suite.durationMs / 1000.0)
                << "\">\n";
            
            // Output test cases for this suite
            auto& suiteTests = testsBySuite[suite.name];
            for (const auto& test : suiteTests) {
                xml << "    <testcase name=\"" << test.testName
                    << "\" status=\"" << test.status
                    << "\" time=\"" << (test.durationMs / 1000.0)
                    << "\"";
                
                if (test.status == "FAILED") {
                    xml << ">\n"
                        << "      <failure message=\"Test failed\">\n"
                        << "        <![CDATA[\n" << test.failureMessage << "\n        ]]>\n"
                        << "      </failure>\n"
                        << "    </testcase>\n";
                } else {
                    xml << "/>\n";
                }
            }
            
            xml << "  </testsuite>\n";
        }
        
        // XML footer
        xml << "</testsuites>\n";
    }
    
    // Generate CSV report
    void generateCSVReport(int64_t totalDurationMs) {
        std::ofstream csv(m_outputDir + "/report.csv");
        
        // CSV header
        csv << "Suite,Test,Status,Duration (ms)\n";
        
        // Output all test results
        for (const auto& test : m_testResults) {
            csv << test.suiteName << ","
                << test.testName << ","
                << test.status << ","
                << test.durationMs << "\n";
        }
    }
    
    // Print summary to console
    void printSummary(int64_t totalDurationMs) {
        // Count total tests and failures
        int totalTests = 0;
        int totalFailures = 0;
        int totalDisabled = 0;
        
        for (const auto& suite : m_suiteResults) {
            totalTests += suite.testCount;
            totalFailures += suite.failedCount;
            totalDisabled += suite.disabledCount;
        }
        
        // Print summary
        std::cout << "\n===== Test Report Summary =====\n";
        std::cout << "Total Test Suites: " << m_suiteResults.size() << "\n";
        std::cout << "Total Tests: " << totalTests << "\n";
        std::cout << "Passed: " << (totalTests - totalFailures - totalDisabled) << "\n";
        std::cout << "Failed: " << totalFailures << "\n";
        std::cout << "Disabled: " << totalDisabled << "\n";
        std::cout << "Pass Rate: " << (totalTests > 0 ? (100.0 * (totalTests - totalFailures - totalDisabled) / totalTests) : 0) << "%\n";
        std::cout << "Total Duration: " << (totalDurationMs / 1000.0) << " seconds\n";
        
        // Print report location
        std::cout << "\nDetailed reports saved to: " << m_outputDir << "\n";
        std::cout << "==============================\n";
    }
    
    // Member variables
    std::string m_outputDir;
    std::chrono::high_resolution_clock::time_point m_startTime;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> m_suiteStartTimes;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> m_testStartTimes;
    std::vector<TestSuiteResult> m_suiteResults;
    std::vector<TestResult> m_testResults;
};

/**
 * @brief Helper to add the test report generator to GoogleTest
 */
inline void EnableTestReporting(const std::string& outputDir = "test_reports") {
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    
    // Delete the default printer
    delete listeners.Release(listeners.default_result_printer());
    
    // Add our custom test report generator
    listeners.Append(new TestReportGenerator(outputDir));
}