#pragma once
#include <string>
#include <chrono>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>
#include <iostream>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stack>
#include <cstdint>
#include <iomanip>
#include <typeinfo>
#include <typeindex>
#include <type_traits>

namespace Performance {

// Forward declarations
class ProfilingManager;

/**
 * @brief Performance metrics for a specific scope or operation
 */
struct ProfileMetrics {
    std::string name;                    // Name of the profiled section
    std::chrono::nanoseconds totalTime{0};  // Total time spent in this section
    std::chrono::nanoseconds minTime{std::chrono::nanoseconds::max()};  // Minimum time for a single call
    std::chrono::nanoseconds maxTime{0};  // Maximum time for a single call
    uint64_t callCount{0};               // Number of times this section was called
    uint64_t frameCount{0};              // Number of frames this section was active
    double averageTimePerCall{0.0};      // Average time per call in ms
    double averageTimePerFrame{0.0};     // Average time per frame in ms
    double lastTimeMs{0.0};              // Most recent execution time in ms
    std::string category;                // Category for grouping metrics
    
    // Performance budget tracking
    double budgetMs{0.0};                // Performance budget in ms (0 = no budget)
    uint64_t budgetExceededCount{0};     // Number of times the budget was exceeded
    
    // Hierarchical tracking
    std::string parentName;              // Parent scope name
    int hierarchyLevel{0};               // Depth in the profiling hierarchy
    
    // Memory tracking
    size_t allocatedBytes{0};            // Bytes allocated during this scope
    size_t deallocatedBytes{0};          // Bytes deallocated during this scope
    int64_t netMemoryChange{0};          // Net memory change (allocated - deallocated)
};

/**
 * @brief RAII class for automatic profiling of a scope
 */
class ScopedProfiler {
public:
    /**
     * @brief Constructor that starts timing a named scope
     * @param name Name of the scope to profile
     * @param category Category for grouping metrics
     */
    ScopedProfiler(const std::string& name, const std::string& category = "Default");
    
    /**
     * @brief Destructor that stops timing and records metrics
     */
    ~ScopedProfiler();
    
    /**
     * @brief Get the name of the profiled scope
     * @return Name of the scope
     */
    const std::string& getName() const { return m_name; }
    
    /**
     * @brief Get the category of the profiled scope
     * @return Category name
     */
    const std::string& getCategory() const { return m_category; }
    
    /**
     * @brief Get the elapsed time so far
     * @return Elapsed time in milliseconds
     */
    double getElapsedMs() const;
    
    /**
     * @brief Manually track a memory allocation
     * @param bytes Number of bytes allocated
     */
    void trackAllocation(size_t bytes);
    
    /**
     * @brief Manually track a memory deallocation
     * @param bytes Number of bytes deallocated
     */
    void trackDeallocation(size_t bytes);
    
private:
    std::string m_name;
    std::string m_category;
    std::chrono::high_resolution_clock::time_point m_startTime;
    size_t m_allocatedBytes{0};
    size_t m_deallocatedBytes{0};
    bool m_active{true};
};

/**
 * @brief Profiler for frame-by-frame metrics
 */
class FrameProfiler {
public:
    /**
     * @brief Constructor that starts timing a frame
     */
    FrameProfiler();
    
    /**
     * @brief Destructor that stops timing and records frame metrics
     */
    ~FrameProfiler();
    
    /**
     * @brief Begin a new frame
     */
    void beginFrame();
    
    /**
     * @brief End the current frame
     * @return Frame time in milliseconds
     */
    double endFrame();
    
    /**
     * @brief Get the current frame number
     * @return Current frame number
     */
    uint64_t getCurrentFrame() const { return m_frameCount; }
    
    /**
     * @brief Get the elapsed time for the current frame
     * @return Elapsed time in milliseconds
     */
    double getElapsedMs() const;
    
    /**
     * @brief Check if a frame is in progress
     * @return True if a frame is being profiled
     */
    bool isFrameActive() const { return m_frameActive; }
    
private:
    std::chrono::high_resolution_clock::time_point m_frameStartTime;
    uint64_t m_frameCount{0};
    bool m_frameActive{false};
};

/**
 * @brief Thread-safe performance metrics reporter
 */
class ProfilingManager {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to the ProfilingManager
     */
    static ProfilingManager& getInstance();
    
    /**
     * @brief Begin profiling a named scope
     * @param name Name of the scope
     * @param category Category for grouping metrics
     */
    void beginScope(const std::string& name, const std::string& category = "Default");
    
    /**
     * @brief End profiling for the current scope
     * @param name Name of the scope
     * @param elapsedNs Elapsed time in nanoseconds
     * @param allocatedBytes Bytes allocated in the scope
     * @param deallocatedBytes Bytes deallocated in the scope
     */
    void endScope(const std::string& name, std::chrono::nanoseconds elapsedNs, 
                 size_t allocatedBytes, size_t deallocatedBytes);
    
    /**
     * @brief Record a frame's metrics
     * @param frameTimeNs Frame time in nanoseconds
     */
    void recordFrameTime(std::chrono::nanoseconds frameTimeNs);
    
    /**
     * @brief Set a performance budget for a scope
     * @param name Name of the scope
     * @param budgetMs Performance budget in milliseconds
     */
    void setPerformanceBudget(const std::string& name, double budgetMs);
    
    /**
     * @brief Get all collected metrics
     * @return Vector of metrics for all profiled scopes
     */
    std::vector<ProfileMetrics> getAllMetrics() const;
    
    /**
     * @brief Get metrics for a specific scope
     * @param name Name of the scope
     * @return Metrics for the scope, or nullptr if not found
     */
    const ProfileMetrics* getMetrics(const std::string& name) const;
    
    /**
     * @brief Reset all metrics
     */
    void resetMetrics();
    
    /**
     * @brief Enable or disable profiling
     * @param enabled Whether profiling should be enabled
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if profiling is enabled
     * @return True if profiling is enabled
     */
    bool isEnabled() const { return m_enabled; }
    
    /**
     * @brief Print all metrics to the console
     * @param sortByTime Whether to sort metrics by total time
     */
    void printMetrics(bool sortByTime = true) const;
    
    /**
     * @brief Export metrics to a CSV file
     * @param filename Filename for the CSV report
     * @return True if export was successful
     */
    bool exportMetricsToCSV(const std::string& filename) const;
    
    /**
     * @brief Get frame statistics
     * @param outAvgFrameTime Average frame time in milliseconds
     * @param outMinFrameTime Minimum frame time in milliseconds
     * @param outMaxFrameTime Maximum frame time in milliseconds
     * @param outStdDev Standard deviation of frame times
     * @param outLastNFrames Number of recent frames to analyze
     */
    void getFrameStats(double& outAvgFrameTime, double& outMinFrameTime, 
                     double& outMaxFrameTime, double& outStdDev, int outLastNFrames = 100) const;
    
    /**
     * @brief Check for performance spikes
     * @param threshold Threshold for detecting spikes (multiple of average)
     * @param windowSize Number of frames to check
     * @return Number of detected spikes
     */
    int detectPerformanceSpikes(double threshold = 2.0, int windowSize = 100) const;
    
    /**
     * @brief Check for performance regressions
     * @param baselineFile Filename of baseline metrics CSV
     * @param thresholdPercent Percent increase to consider a regression
     * @return Number of detected regressions
     */
    int detectRegressions(const std::string& baselineFile, double thresholdPercent = 10.0) const;
    
    /**
     * @brief Save current metrics as baseline
     * @param filename Filename for baseline metrics
     * @return True if save was successful
     */
    bool saveAsBaseline(const std::string& filename) const;
    
    /**
     * @brief Get current active scopes for hierarchical profiling
     * @return Stack of scope names
     */
    const std::stack<std::string>& getCurrentScopeStack() const { return m_scopeStack; }
    
    /**
     * @brief Track an allocation in the current scope
     * @param bytes Number of bytes allocated
     */
    void trackAllocation(size_t bytes);
    
    /**
     * @brief Track a deallocation in the current scope
     * @param bytes Number of bytes deallocated
     */
    void trackDeallocation(size_t bytes);
    
    /**
     * @brief Get the current frame profiler
     * @return Reference to the frame profiler
     */
    FrameProfiler& getFrameProfiler() { return m_frameProfiler; }
    
    /**
     * @brief Register a callback for budget exceeded events
     * @param callback Function to call when a budget is exceeded
     */
    void setBudgetExceededCallback(std::function<void(const std::string&, double, double)> callback) {
        m_budgetExceededCallback = callback;
    }
    
private:
    // Private constructor for singleton
    ProfilingManager();
    ~ProfilingManager() = default;
    
    // Thread safety
    mutable std::mutex m_metricsMutex;
    
    // Metrics storage
    std::unordered_map<std::string, ProfileMetrics> m_metrics;
    
    // Frame time tracking
    std::vector<double> m_frameTimesMs;
    std::chrono::nanoseconds m_totalFrameTime{0};
    FrameProfiler m_frameProfiler;
    
    // Current scope stack for hierarchical profiling
    std::stack<std::string> m_scopeStack;
    std::unordered_map<std::string, size_t> m_scopeAllocations;
    std::unordered_map<std::string, size_t> m_scopeDeallocations;
    
    // Configuration
    bool m_enabled{true};
    size_t m_maxFrameHistory{1000};
    
    // Callbacks
    std::function<void(const std::string&, double, double)> m_budgetExceededCallback;
    
    // Helper methods
    void checkPerformanceBudget(const std::string& name, double timeMs);
};

/**
 * @brief Template class for type-specific performance tracking
 */
template<typename T>
class TypedProfiler {
public:
    /**
     * @brief Begin profiling for a specific operation on type T
     * @param operation Name of the operation
     * @return Unique pointer to a ScopedProfiler
     */
    static std::unique_ptr<ScopedProfiler> begin(const std::string& operation) {
        std::string typeName = typeid(T).name();
        std::string name = typeName + "::" + operation;
        return std::make_unique<ScopedProfiler>(name, "Type");
    }
    
    /**
     * @brief Set performance budget for an operation on type T
     * @param operation Name of the operation
     * @param budgetMs Performance budget in milliseconds
     */
    static void setPerformanceBudget(const std::string& operation, double budgetMs) {
        std::string typeName = typeid(T).name();
        std::string name = typeName + "::" + operation;
        ProfilingManager::getInstance().setPerformanceBudget(name, budgetMs);
    }
};

// Convenience macros for profiling
#define PROFILE_SCOPE(name) \
    Performance::ScopedProfiler __profiler##__LINE__(name)

#define PROFILE_SCOPE_CAT(name, category) \
    Performance::ScopedProfiler __profiler##__LINE__(name, category)

#define PROFILE_FUNCTION() \
    Performance::ScopedProfiler __profiler##__LINE__(__FUNCTION__)

#define PROFILE_FRAME() \
    Performance::FrameProfiler __frameProfiler##__LINE__

#define PROFILE_BEGIN_FRAME() \
    Performance::ProfilingManager::getInstance().getFrameProfiler().beginFrame()

#define PROFILE_END_FRAME() \
    Performance::ProfilingManager::getInstance().getFrameProfiler().endFrame()

#define PROFILE_TYPE_OPERATION(type, operation) \
    auto __typedProfiler##__LINE__ = Performance::TypedProfiler<type>::begin(operation)

} // namespace Performance