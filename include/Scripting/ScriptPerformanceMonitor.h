#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <mutex>
#include <optional>
#include <functional>
#include <chrono>

namespace Scripting {

/**
 * @brief Performance monitoring system for scripts
 * 
 * This class tracks the performance of script execution, identifies 
 * hotspots, and provides alerts when performance budgets are exceeded.
 */
class ScriptPerformanceMonitor {
public:
    /**
     * @brief Performance sample data
     */
    struct PerformanceSample {
        std::chrono::system_clock::time_point timestamp; ///< When the sample was taken
        double totalExecutionTime = 0.0;                 ///< Total script execution time in ms
        std::map<std::string, double> functionTimes;     ///< Individual function times in ms
    };
    
    /**
     * @brief Function performance statistics
     */
    struct FunctionStatistics {
        double avgExecutionTime = 0.0;    ///< Average execution time in ms
        double maxExecutionTime = 0.0;    ///< Maximum execution time in ms
        double totalExecutionTime = 0.0;  ///< Total execution time in ms
        size_t callCount = 0;             ///< Number of times the function was called
    };
    
    /**
     * @brief Performance alert types
     */
    enum class AlertType {
        BudgetExceeded,  ///< Performance budget exceeded
        Hotspot,         ///< Performance hotspot detected
        HighMemoryUsage  ///< High memory usage detected
    };
    
    /**
     * @brief Performance alert information
     */
    struct PerformanceAlert {
        AlertType type;             ///< Type of alert
        std::string functionName;   ///< Function name
        double value;               ///< Current value
        double threshold;           ///< Threshold that was exceeded
        std::string message;        ///< Alert message
    };
    
    /**
     * @brief Performance alert callback type
     */
    using AlertCallback = std::function<void(const PerformanceAlert&)>;
    
    /**
     * @brief Get the singleton instance
     * @return Reference to the script performance monitor
     */
    static ScriptPerformanceMonitor& getInstance();
    
    /**
     * @brief Initialize the performance monitor
     * @return True if initialization succeeds
     */
    bool initialize();
    
    /**
     * @brief Shutdown the performance monitor
     */
    void shutdown();
    
    /**
     * @brief Update the performance monitor
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Take a performance sample manually
     */
    void takeSample();
    
    /**
     * @brief Set the sampling interval
     * @param intervalSeconds Interval in seconds
     */
    void setSamplingInterval(float intervalSeconds);
    
    /**
     * @brief Set the history size
     * @param size Number of samples to keep
     */
    void setHistorySize(size_t size);
    
    /**
     * @brief Set the hotspot threshold
     * @param thresholdMs Threshold in milliseconds
     */
    void setHotspotThreshold(double thresholdMs);
    
    /**
     * @brief Enable or disable the monitor
     * @param enabled Whether monitoring should be enabled
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if the monitor is enabled
     * @return True if monitoring is enabled
     */
    bool isEnabled() const { return m_enabled; }
    
    /**
     * @brief Get the current performance sample
     * @return Current performance sample
     */
    PerformanceSample getCurrentSample() const;
    
    /**
     * @brief Get the performance history
     * @return Vector of performance samples
     */
    std::vector<PerformanceSample> getPerformanceHistory() const;
    
    /**
     * @brief Get all function statistics
     * @return Map of function names to statistics
     */
    std::map<std::string, FunctionStatistics> getAllFunctionStats() const;
    
    /**
     * @brief Get statistics for a specific function
     * @param functionName Name of the function
     * @return Function statistics if found
     */
    std::optional<FunctionStatistics> getFunctionStats(const std::string& functionName) const;
    
    /**
     * @brief Get functions identified as hotspots
     * @return Vector of hotspot function names
     */
    std::vector<std::string> getHotspots() const;
    
    /**
     * @brief Reset all statistics
     */
    void resetStats();
    
    /**
     * @brief Export statistics to CSV file
     * @param filename Path to output file
     * @return True if export succeeds
     */
    bool exportStatsToCSV(const std::string& filename) const;
    
    /**
     * @brief Export performance history to CSV file
     * @param filename Path to output file
     * @return True if export succeeds
     */
    bool exportHistoryToCSV(const std::string& filename) const;
    
    /**
     * @brief Get the top N functions by execution time
     * @param n Number of functions to get
     * @param useAverage Whether to use average time (true) or total time (false)
     * @return Vector of function names and execution times
     */
    std::vector<std::pair<std::string, double>> getTopFunctions(size_t n, bool useAverage = true) const;
    
    /**
     * @brief Get function performance budgets
     * @return Map of function names to budget times in milliseconds
     */
    std::map<std::string, double> getFunctionBudgets() const;
    
    /**
     * @brief Set a function performance budget
     * @param functionName Name of the function
     * @param budgetMs Budget in milliseconds
     */
    void setFunctionBudget(const std::string& functionName, double budgetMs);
    
    /**
     * @brief Remove a function performance budget
     * @param functionName Name of the function
     */
    void removeFunctionBudget(const std::string& functionName);
    
    /**
     * @brief Get functions that exceed their performance budget
     * @return Vector of function names and overrun amounts
     */
    std::vector<std::pair<std::string, double>> getBudgetOverruns() const;
    
    /**
     * @brief Register a performance alert callback
     * @param callback Function to call when an alert is triggered
     * @return ID to use for unregistering the callback
     */
    int registerAlertCallback(AlertCallback callback);
    
    /**
     * @brief Unregister a performance alert callback
     * @param callbackId ID returned from registerAlertCallback
     */
    void unregisterAlertCallback(int callbackId);
    
    /**
     * @brief Process alerts based on current performance data
     */
    void processAlerts();
    
private:
    // Private constructor for singleton
    ScriptPerformanceMonitor();
    ~ScriptPerformanceMonitor();
    
    // Prevent copying or moving
    ScriptPerformanceMonitor(const ScriptPerformanceMonitor&) = delete;
    ScriptPerformanceMonitor& operator=(const ScriptPerformanceMonitor&) = delete;
    ScriptPerformanceMonitor(ScriptPerformanceMonitor&&) = delete;
    ScriptPerformanceMonitor& operator=(ScriptPerformanceMonitor&&) = delete;
    
    // Member variables
    bool m_enabled;                                          ///< Whether monitoring is enabled
    float m_samplingInterval;                                ///< Interval between samples in seconds
    float m_timeSinceLastSample;                             ///< Time since last sample
    size_t m_historySize;                                    ///< Number of samples to keep
    double m_hotspotThreshold = 10.0;                        ///< Threshold for hotspots in milliseconds
    
    std::deque<PerformanceSample> m_performanceHistory;      ///< Performance sample history
    std::map<std::string, FunctionStatistics> m_functionStats; ///< Function statistics
    std::set<std::string> m_hotspots;                        ///< Functions identified as hotspots
    std::map<std::string, double> m_functionBudgets;         ///< Function performance budgets
    
    PerformanceSample m_currentSample;                       ///< Most recent sample
    
    std::map<int, AlertCallback> m_alertCallbacks;           ///< Performance alert callbacks
    int m_nextCallbackId = 1;                                ///< Next callback ID
    
    // Thread safety
    mutable std::mutex m_mutex;                              ///< Mutex for thread safety
};

} // namespace Scripting