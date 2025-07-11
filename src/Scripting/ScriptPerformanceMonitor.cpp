#include "Scripting/ScriptPerformanceMonitor.h"
#include "Scripting/ScriptManager.h"
#include "Scripting/IScriptEngine.h"
#include <algorithm>
#include <chrono>
#include <numeric>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace Scripting {

// Constructor
ScriptPerformanceMonitor::ScriptPerformanceMonitor()
    : m_enabled(false)
    , m_samplingInterval(1.0f)
    , m_timeSinceLastSample(0.0f)
    , m_historySize(60)  // 1 minute of history at 1 sample per second
{
}

// Destructor
ScriptPerformanceMonitor::~ScriptPerformanceMonitor() {
    // Clear any resources
}

// Singleton instance
ScriptPerformanceMonitor& ScriptPerformanceMonitor::getInstance() {
    static ScriptPerformanceMonitor instance;
    return instance;
}

// Initialize the monitor
bool ScriptPerformanceMonitor::initialize() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_enabled = true;
    m_timeSinceLastSample = 0.0f;
    m_performanceHistory.clear();
    
    return true;
}

// Shutdown the monitor
void ScriptPerformanceMonitor::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_enabled = false;
    m_performanceHistory.clear();
    m_currentSample = PerformanceSample();
}

// Update the monitor
void ScriptPerformanceMonitor::update(float deltaTime) {
    if (!m_enabled) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Add deltaTime to accumulator
    m_timeSinceLastSample += deltaTime;
    
    // Check if it's time to take a sample
    if (m_timeSinceLastSample >= m_samplingInterval) {
        takeSample();
        m_timeSinceLastSample = 0.0f;
    }
}

// Take a performance sample
void ScriptPerformanceMonitor::takeSample() {
    // Get the script manager
    auto& scriptManager = ScriptManager::getInstance();
    
    // Get profiling results from all engines
    auto profilingResults = scriptManager.getProfilingResults();
    
    // Create a new sample
    PerformanceSample sample;
    sample.timestamp = std::chrono::system_clock::now();
    
    // Process profiling results
    double totalTime = 0.0;
    for (const auto& [engineName, engineResults] : profilingResults) {
        for (const auto& [functionName, executionTime] : engineResults) {
            totalTime += executionTime;
            
            // Store function execution time
            sample.functionTimes[functionName] = executionTime;
            
            // Update max time for this function
            auto& functionStats = m_functionStats[functionName];
            functionStats.maxExecutionTime = std::max(functionStats.maxExecutionTime, executionTime);
            functionStats.totalExecutionTime += executionTime;
            functionStats.callCount++;
            functionStats.avgExecutionTime = functionStats.totalExecutionTime / functionStats.callCount;
            
            // Check if this function is a hotspot
            if (executionTime > m_hotspotThreshold) {
                m_hotspots.insert(functionName);
            }
        }
    }
    
    // Store total execution time
    sample.totalExecutionTime = totalTime;
    
    // Add to history
    m_performanceHistory.push_back(sample);
    
    // Trim history if too large
    while (m_performanceHistory.size() > m_historySize) {
        m_performanceHistory.pop_front();
    }
    
    // Store current sample for quick access
    m_currentSample = sample;
}

// Set the sampling interval
void ScriptPerformanceMonitor::setSamplingInterval(float intervalSeconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_samplingInterval = std::max(0.1f, intervalSeconds);  // Minimum 100ms
}

// Set the history size
void ScriptPerformanceMonitor::setHistorySize(size_t size) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_historySize = std::max(size_t(10), size);
    
    // Trim history if necessary
    while (m_performanceHistory.size() > m_historySize) {
        m_performanceHistory.pop_front();
    }
}

// Set the hotspot threshold
void ScriptPerformanceMonitor::setHotspotThreshold(double thresholdMs) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_hotspotThreshold = thresholdMs;
    
    // Re-evaluate hotspots
    m_hotspots.clear();
    for (const auto& [functionName, stats] : m_functionStats) {
        if (stats.maxExecutionTime > m_hotspotThreshold) {
            m_hotspots.insert(functionName);
        }
    }
}

// Enable/disable the monitor
void ScriptPerformanceMonitor::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_enabled != enabled) {
        m_enabled = enabled;
        
        if (enabled) {
            // Enable profiling in script engines
            auto& scriptManager = ScriptManager::getInstance();
            scriptManager.enableProfiling(true);
        }
        else {
            // Disable profiling in script engines
            auto& scriptManager = ScriptManager::getInstance();
            scriptManager.enableProfiling(false);
        }
    }
}

// Get the current performance sample
ScriptPerformanceMonitor::PerformanceSample ScriptPerformanceMonitor::getCurrentSample() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentSample;
}

// Get the performance history
std::vector<ScriptPerformanceMonitor::PerformanceSample> ScriptPerformanceMonitor::getPerformanceHistory() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::vector<PerformanceSample>(m_performanceHistory.begin(), m_performanceHistory.end());
}

// Get all function statistics
std::map<std::string, ScriptPerformanceMonitor::FunctionStatistics> ScriptPerformanceMonitor::getAllFunctionStats() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_functionStats;
}

// Get stats for a specific function
std::optional<ScriptPerformanceMonitor::FunctionStatistics> ScriptPerformanceMonitor::getFunctionStats(const std::string& functionName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_functionStats.find(functionName);
    if (it != m_functionStats.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

// Get hotspots
std::vector<std::string> ScriptPerformanceMonitor::getHotspots() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::vector<std::string>(m_hotspots.begin(), m_hotspots.end());
}

// Reset all statistics
void ScriptPerformanceMonitor::resetStats() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_performanceHistory.clear();
    m_functionStats.clear();
    m_hotspots.clear();
    m_currentSample = PerformanceSample();
    
    // Reset profiling data in script engines
    auto& scriptManager = ScriptManager::getInstance();
    scriptManager.resetProfilingData();
}

// Export statistics to CSV
bool ScriptPerformanceMonitor::exportStatsToCSV(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "Function,Average Time (ms),Max Time (ms),Total Time (ms),Call Count\n";
    
    // Write function stats
    for (const auto& [functionName, stats] : m_functionStats) {
        file << "\"" << functionName << "\","
             << stats.avgExecutionTime << ","
             << stats.maxExecutionTime << ","
             << stats.totalExecutionTime << ","
             << stats.callCount << "\n";
    }
    
    file.close();
    return true;
}

// Export performance history to CSV
bool ScriptPerformanceMonitor::exportHistoryToCSV(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Get all unique function names
    std::set<std::string> functionNames;
    for (const auto& sample : m_performanceHistory) {
        for (const auto& [name, _] : sample.functionTimes) {
            functionNames.insert(name);
        }
    }
    
    // Write header
    file << "Timestamp,Total Time (ms)";
    for (const auto& name : functionNames) {
        file << ",\"" << name << "\"";
    }
    file << "\n";
    
    // Write samples
    for (const auto& sample : m_performanceHistory) {
        // Convert timestamp to string
        auto time_t = std::chrono::system_clock::to_time_t(sample.timestamp);
        std::tm tm;
        #ifdef _WIN32
        localtime_s(&tm, &time_t);
        #else
        tm = *std::localtime(&time_t);
        #endif
        std::ostringstream timeStr;
        timeStr << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        
        file << "\"" << timeStr.str() << "\"," << sample.totalExecutionTime;
        
        // Write function times
        for (const auto& functionName : functionNames) {
            auto it = sample.functionTimes.find(functionName);
            if (it != sample.functionTimes.end()) {
                file << "," << it->second;
            } else {
                file << ",0.0";
            }
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

// Get the top N functions by execution time
std::vector<std::pair<std::string, double>> ScriptPerformanceMonitor::getTopFunctions(size_t n, bool useAverage) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::pair<std::string, double>> functions;
    functions.reserve(m_functionStats.size());
    
    for (const auto& [name, stats] : m_functionStats) {
        if (useAverage) {
            functions.emplace_back(name, stats.avgExecutionTime);
        } else {
            functions.emplace_back(name, stats.totalExecutionTime);
        }
    }
    
    // Sort by execution time (descending)
    std::sort(functions.begin(), functions.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Limit to top N
    if (functions.size() > n) {
        functions.resize(n);
    }
    
    return functions;
}

// Get function budgets
std::map<std::string, double> ScriptPerformanceMonitor::getFunctionBudgets() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_functionBudgets;
}

// Set a function budget
void ScriptPerformanceMonitor::setFunctionBudget(const std::string& functionName, double budgetMs) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_functionBudgets[functionName] = budgetMs;
}

// Remove a function budget
void ScriptPerformanceMonitor::removeFunctionBudget(const std::string& functionName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_functionBudgets.erase(functionName);
}

// Get budget overruns
std::vector<std::pair<std::string, double>> ScriptPerformanceMonitor::getBudgetOverruns() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::pair<std::string, double>> overruns;
    
    for (const auto& [functionName, budget] : m_functionBudgets) {
        auto it = m_functionStats.find(functionName);
        if (it != m_functionStats.end()) {
            const auto& stats = it->second;
            if (stats.avgExecutionTime > budget) {
                overruns.emplace_back(functionName, stats.avgExecutionTime - budget);
            }
        }
    }
    
    // Sort by overrun amount (descending)
    std::sort(overruns.begin(), overruns.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return overruns;
}

// Register a performance alert callback
int ScriptPerformanceMonitor::registerAlertCallback(AlertCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    int id = m_nextCallbackId++;
    m_alertCallbacks[id] = callback;
    return id;
}

// Unregister a performance alert callback
void ScriptPerformanceMonitor::unregisterAlertCallback(int callbackId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_alertCallbacks.erase(callbackId);
}

// Process alerts based on current performance data
void ScriptPerformanceMonitor::processAlerts() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check for budget overruns
    for (const auto& [functionName, budget] : m_functionBudgets) {
        auto it = m_functionStats.find(functionName);
        if (it != m_functionStats.end()) {
            const auto& stats = it->second;
            if (stats.avgExecutionTime > budget) {
                // Create alert
                PerformanceAlert alert;
                alert.type = AlertType::BudgetExceeded;
                alert.functionName = functionName;
                alert.value = stats.avgExecutionTime;
                alert.threshold = budget;
                alert.message = "Budget exceeded for " + functionName + ": " +
                    std::to_string(stats.avgExecutionTime) + " ms (budget: " +
                    std::to_string(budget) + " ms)";
                
                // Notify callbacks
                for (const auto& [id, callback] : m_alertCallbacks) {
                    callback(alert);
                }
            }
        }
    }
    
    // Check for hotspots
    for (const std::string& functionName : m_hotspots) {
        auto it = m_functionStats.find(functionName);
        if (it != m_functionStats.end() && it->second.maxExecutionTime > m_hotspotThreshold) {
            // Create alert
            PerformanceAlert alert;
            alert.type = AlertType::Hotspot;
            alert.functionName = functionName;
            alert.value = it->second.maxExecutionTime;
            alert.threshold = m_hotspotThreshold;
            alert.message = "Hotspot detected: " + functionName + " took " +
                std::to_string(it->second.maxExecutionTime) + " ms";
            
            // Notify callbacks
            for (const auto& [id, callback] : m_alertCallbacks) {
                callback(alert);
            }
        }
    }
}

} // namespace Scripting