#include "Performance/Profiler.h"

namespace Performance {

ScopedProfiler::ScopedProfiler(const std::string& name, const std::string& category)
    : m_name(name), m_category(category), m_startTime(std::chrono::high_resolution_clock::now())
{
    ProfilingManager::getInstance().beginScope(name, category);
}

ScopedProfiler::~ScopedProfiler() {
    if (m_active) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = endTime - m_startTime;
        ProfilingManager::getInstance().endScope(m_name, elapsedTime, m_allocatedBytes, m_deallocatedBytes);
    }
}

double ScopedProfiler::getElapsedMs() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = now - m_startTime;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() / 1000000.0;
}

void ScopedProfiler::trackAllocation(size_t bytes) {
    m_allocatedBytes += bytes;
    ProfilingManager::getInstance().trackAllocation(bytes);
}

void ScopedProfiler::trackDeallocation(size_t bytes) {
    m_deallocatedBytes += bytes;
    ProfilingManager::getInstance().trackDeallocation(bytes);
}

FrameProfiler::FrameProfiler() {
    beginFrame();
}

FrameProfiler::~FrameProfiler() {
    if (m_frameActive) {
        endFrame();
    }
}

void FrameProfiler::beginFrame() {
    m_frameActive = true;
    m_frameStartTime = std::chrono::high_resolution_clock::now();
}

double FrameProfiler::endFrame() {
    if (!m_frameActive) {
        return 0.0;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsed = endTime - m_frameStartTime;
    auto elapsedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
    
    m_frameActive = false;
    m_frameCount++;
    
    // Record frame time
    ProfilingManager::getInstance().recordFrameTime(elapsedNs);
    
    return elapsedNs.count() / 1000000.0; // Convert to milliseconds
}

double FrameProfiler::getElapsedMs() const {
    if (!m_frameActive) {
        return 0.0;
    }
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = now - m_frameStartTime;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() / 1000000.0;
}

ProfilingManager& ProfilingManager::getInstance() {
    static ProfilingManager instance;
    return instance;
}

ProfilingManager::ProfilingManager()
    : m_frameTimesMs(m_maxFrameHistory, 0.0)
{
}

void ProfilingManager::beginScope(const std::string& name, const std::string& category) {
    if (!m_enabled) {
        return;
    }
    
    // Create metrics entry if it doesn't exist
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    // Update hierarchical information
    int hierarchyLevel = m_scopeStack.size();
    std::string parentName;
    
    if (!m_scopeStack.empty()) {
        parentName = m_scopeStack.top();
    }
    
    m_scopeStack.push(name);
    
    // Initialize metrics if needed
    if (m_metrics.find(name) == m_metrics.end()) {
        ProfileMetrics metrics;
        metrics.name = name;
        metrics.category = category;
        metrics.hierarchyLevel = hierarchyLevel;
        metrics.parentName = parentName;
        m_metrics[name] = metrics;
    }
    
    // Initialize memory tracking for this scope
    m_scopeAllocations[name] = 0;
    m_scopeDeallocations[name] = 0;
}

void ProfilingManager::endScope(const std::string& name, std::chrono::nanoseconds elapsedNs,
                              size_t allocatedBytes, size_t deallocatedBytes) {
    if (!m_enabled) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    // Remove from scope stack if it's at the top
    if (!m_scopeStack.empty() && m_scopeStack.top() == name) {
        m_scopeStack.pop();
    }
    
    // Combine explicit and tracked memory changes
    if (m_scopeAllocations.find(name) != m_scopeAllocations.end()) {
        allocatedBytes += m_scopeAllocations[name];
    }
    
    if (m_scopeDeallocations.find(name) != m_scopeDeallocations.end()) {
        deallocatedBytes += m_scopeDeallocations[name];
    }
    
    // Clear memory tracking for this scope
    m_scopeAllocations.erase(name);
    m_scopeDeallocations.erase(name);
    
    // Update metrics
    auto& metrics = m_metrics[name];
    metrics.totalTime += elapsedNs;
    metrics.minTime = std::min(metrics.minTime, elapsedNs);
    metrics.maxTime = std::max(metrics.maxTime, elapsedNs);
    metrics.callCount++;
    
    // Frame tracking
    bool isNewFrameForMetric = (metrics.frameCount < m_frameProfiler.getCurrentFrame());
    if (isNewFrameForMetric) {
        metrics.frameCount++;
    }
    
    // Memory tracking
    metrics.allocatedBytes += allocatedBytes;
    metrics.deallocatedBytes += deallocatedBytes;
    metrics.netMemoryChange = static_cast<int64_t>(metrics.allocatedBytes - metrics.deallocatedBytes);
    
    // Update time averages
    metrics.averageTimePerCall = metrics.totalTime.count() / static_cast<double>(metrics.callCount) / 1000000.0;
    metrics.averageTimePerFrame = metrics.totalTime.count() / static_cast<double>(metrics.frameCount ? metrics.frameCount : 1) / 1000000.0;
    
    // Record last time
    double timeMs = elapsedNs.count() / 1000000.0;
    metrics.lastTimeMs = timeMs;
    
    // Check performance budget
    checkPerformanceBudget(name, timeMs);
}

void ProfilingManager::recordFrameTime(std::chrono::nanoseconds frameTimeNs) {
    if (!m_enabled) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    // Convert to milliseconds
    double frameTimeMs = frameTimeNs.count() / 1000000.0;
    
    // Update frame time history
    if (m_frameTimesMs.size() >= m_maxFrameHistory) {
        m_frameTimesMs.erase(m_frameTimesMs.begin());
    }
    m_frameTimesMs.push_back(frameTimeMs);
    
    // Update total frame time
    m_totalFrameTime += frameTimeNs;
}

void ProfilingManager::setPerformanceBudget(const std::string& name, double budgetMs) {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    // Find or create the metrics entry
    auto& metrics = m_metrics[name];
    if (metrics.name.empty()) {
        metrics.name = name;
    }
    
    metrics.budgetMs = budgetMs;
}

std::vector<ProfileMetrics> ProfilingManager::getAllMetrics() const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    std::vector<ProfileMetrics> result;
    result.reserve(m_metrics.size());
    
    for (const auto& pair : m_metrics) {
        result.push_back(pair.second);
    }
    
    return result;
}

const ProfileMetrics* ProfilingManager::getMetrics(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    auto it = m_metrics.find(name);
    if (it != m_metrics.end()) {
        return &it->second;
    }
    
    return nullptr;
}

void ProfilingManager::resetMetrics() {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    // Clear all metrics
    m_metrics.clear();
    m_frameTimesMs.clear();
    m_frameTimesMs.resize(m_maxFrameHistory, 0.0);
    m_totalFrameTime = std::chrono::nanoseconds(0);
    
    std::cout << "[ProfilingManager] Metrics reset" << std::endl;
}

void ProfilingManager::setEnabled(bool enabled) {
    m_enabled = enabled;
    std::cout << "[ProfilingManager] " << (enabled ? "Enabled" : "Disabled") << std::endl;
}

void ProfilingManager::printMetrics(bool sortByTime) const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    std::cout << "\n===== Performance Metrics Report =====" << std::endl;
    
    // Get all metrics and sort if requested
    std::vector<std::reference_wrapper<const ProfileMetrics>> sortedMetrics;
    for (const auto& pair : m_metrics) {
        sortedMetrics.push_back(std::cref(pair.second));
    }
    
    if (sortByTime) {
        std::sort(sortedMetrics.begin(), sortedMetrics.end(),
                 [](const ProfileMetrics& a, const ProfileMetrics& b) {
                     return a.totalTime > b.totalTime;
                 });
    }
    
    // Calculate total time across all metrics
    std::chrono::nanoseconds totalTime(0);
    for (const auto& metrics : sortedMetrics) {
        totalTime += metrics.get().totalTime;
    }
    
    // Print frame statistics
    double avgFrameTime = 0.0;
    double minFrameTime = 0.0;
    double maxFrameTime = 0.0;
    double stdDev = 0.0;
    getFrameStats(avgFrameTime, minFrameTime, maxFrameTime, stdDev);
    
    std::cout << "Frame Statistics:" << std::endl;
    std::cout << "  Average: " << std::fixed << std::setprecision(3) << avgFrameTime << " ms" << std::endl;
    std::cout << "  Min: " << std::fixed << std::setprecision(3) << minFrameTime << " ms" << std::endl;
    std::cout << "  Max: " << std::fixed << std::setprecision(3) << maxFrameTime << " ms" << std::endl;
    std::cout << "  Std Dev: " << std::fixed << std::setprecision(3) << stdDev << " ms" << std::endl;
    std::cout << "  FPS: " << std::fixed << std::setprecision(1) << (avgFrameTime > 0 ? 1000.0 / avgFrameTime : 0) << std::endl;
    std::cout << std::endl;
    
    // Print headers
    std::cout << std::left << std::setw(30) << "Scope Name";
    std::cout << std::right << std::setw(12) << "Total (ms)";
    std::cout << std::right << std::setw(12) << "Avg (ms)";
    std::cout << std::right << std::setw(12) << "Min (ms)";
    std::cout << std::right << std::setw(12) << "Max (ms)";
    std::cout << std::right << std::setw(12) << "Calls";
    std::cout << std::right << std::setw(10) << "% Time";
    std::cout << std::right << std::setw(15) << "Memory Delta";
    std::cout << std::endl;
    
    std::cout << std::string(115, '-') << std::endl;
    
    // Print metrics
    for (const auto& metricsRef : sortedMetrics) {
        const auto& metrics = metricsRef.get();
        
        // Calculate percentage of total time
        double percentage = 0.0;
        if (totalTime.count() > 0) {
            percentage = 100.0 * metrics.totalTime.count() / static_cast<double>(totalTime.count());
        }
        
        // Format the name with indentation based on hierarchy
        std::string indentedName = std::string(metrics.hierarchyLevel * 2, ' ') + metrics.name;
        
        // Format time values
        double totalTimeMs = metrics.totalTime.count() / 1000000.0;
        double minTimeMs = metrics.minTime.count() / 1000000.0;
        double maxTimeMs = metrics.maxTime.count() / 1000000.0;
        
        std::cout << std::left << std::setw(30) << indentedName;
        std::cout << std::right << std::setw(12) << std::fixed << std::setprecision(3) << totalTimeMs;
        std::cout << std::right << std::setw(12) << std::fixed << std::setprecision(3) << metrics.averageTimePerCall;
        std::cout << std::right << std::setw(12) << std::fixed << std::setprecision(3) << minTimeMs;
        std::cout << std::right << std::setw(12) << std::fixed << std::setprecision(3) << maxTimeMs;
        std::cout << std::right << std::setw(12) << metrics.callCount;
        std::cout << std::right << std::setw(9) << std::fixed << std::setprecision(1) << percentage << "%";
        std::cout << std::right << std::setw(15) << metrics.netMemoryChange;
        
        // Show budget warning if applicable
        if (metrics.budgetMs > 0 && metrics.budgetExceededCount > 0) {
            std::cout << " [Budget exceeded " << metrics.budgetExceededCount << " times]";
        }
        
        std::cout << std::endl;
    }
    
    std::cout << "=================================" << std::endl;
}

bool ProfilingManager::exportMetricsToCSV(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for export: " << filename << std::endl;
            return false;
        }
        
        // Write header
        file << "Name,Category,HierarchyLevel,Parent,TotalTime_ms,AvgTime_ms,MinTime_ms,MaxTime_ms,"
             << "CallCount,FrameCount,Budget_ms,BudgetExceeded,AllocatedBytes,DeallocatedBytes,NetMemoryChange\n";
        
        // Write metrics
        for (const auto& pair : m_metrics) {
            const auto& metrics = pair.second;
            
            double totalTimeMs = metrics.totalTime.count() / 1000000.0;
            double minTimeMs = metrics.minTime.count() / 1000000.0;
            double maxTimeMs = metrics.maxTime.count() / 1000000.0;
            
            file << metrics.name << ","
                 << metrics.category << ","
                 << metrics.hierarchyLevel << ","
                 << metrics.parentName << ","
                 << totalTimeMs << ","
                 << metrics.averageTimePerCall << ","
                 << minTimeMs << ","
                 << maxTimeMs << ","
                 << metrics.callCount << ","
                 << metrics.frameCount << ","
                 << metrics.budgetMs << ","
                 << metrics.budgetExceededCount << ","
                 << metrics.allocatedBytes << ","
                 << metrics.deallocatedBytes << ","
                 << metrics.netMemoryChange << "\n";
        }
        
        // Write frame times
        file << "\nFrame Times (ms)\n";
        for (double frameTime : m_frameTimesMs) {
            if (frameTime > 0) {
                file << frameTime << "\n";
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during metrics export: " << e.what() << std::endl;
        return false;
    }
}

void ProfilingManager::getFrameStats(double& outAvgFrameTime, double& outMinFrameTime, 
                                   double& outMaxFrameTime, double& outStdDev, 
                                   int outLastNFrames) const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    // Initialize outputs
    outAvgFrameTime = 0.0;
    outMinFrameTime = std::numeric_limits<double>::max();
    outMaxFrameTime = 0.0;
    outStdDev = 0.0;
    
    // Limit the number of frames to analyze
    int frameCount = 0;
    int startIdx = std::max<int>(0, static_cast<int>(m_frameTimesMs.size()) - outLastNFrames);
    
    // Calculate min, max, and average
    double sum = 0.0;
    for (size_t i = startIdx; i < m_frameTimesMs.size(); ++i) {
        double frameTime = m_frameTimesMs[i];
        if (frameTime <= 0.0) continue; // Skip invalid entries
        
        sum += frameTime;
        outMinFrameTime = std::min(outMinFrameTime, frameTime);
        outMaxFrameTime = std::max(outMaxFrameTime, frameTime);
        frameCount++;
    }
    
    if (frameCount > 0) {
        outAvgFrameTime = sum / frameCount;
        
        // Calculate standard deviation
        double sumSquaredDiffs = 0.0;
        for (size_t i = startIdx; i < m_frameTimesMs.size(); ++i) {
            double frameTime = m_frameTimesMs[i];
            if (frameTime <= 0.0) continue;
            
            double diff = frameTime - outAvgFrameTime;
            sumSquaredDiffs += diff * diff;
        }
        
        outStdDev = std::sqrt(sumSquaredDiffs / frameCount);
    }
    else {
        // No valid frame times
        outMinFrameTime = 0.0;
    }
}

int ProfilingManager::detectPerformanceSpikes(double threshold, int windowSize) const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    // Get frame statistics
    double avgFrameTime, minFrameTime, maxFrameTime, stdDev;
    getFrameStats(avgFrameTime, minFrameTime, maxFrameTime, stdDev, windowSize);
    
    if (avgFrameTime <= 0.0) {
        return 0; // No valid data
    }
    
    // Count frames exceeding the threshold
    int spikeCount = 0;
    double spikeThreshold = avgFrameTime * threshold;
    
    int startIdx = std::max<int>(0, static_cast<int>(m_frameTimesMs.size()) - windowSize);
    for (size_t i = startIdx; i < m_frameTimesMs.size(); ++i) {
        if (m_frameTimesMs[i] > spikeThreshold) {
            spikeCount++;
        }
    }
    
    return spikeCount;
}

int ProfilingManager::detectRegressions(const std::string& baselineFile, double thresholdPercent) const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    try {
        std::ifstream file(baselineFile);
        if (!file.is_open()) {
            std::cerr << "Failed to open baseline file: " << baselineFile << std::endl;
            return -1;
        }
        
        // Read header
        std::string header;
        std::getline(file, header);
        
        // Read baseline metrics
        std::unordered_map<std::string, double> baselineTimes;
        std::string line;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '\n') {
                break; // End of metrics section
            }
            
            std::stringstream ss(line);
            std::string name, category, parent;
            int hierarchyLevel, callCount, frameCount, budgetExceeded;
            double totalTime, avgTime, minTime, maxTime, budget;
            size_t allocatedBytes, deallocatedBytes;
            int64_t netMemoryChange;
            
            // Parse CSV line
            std::getline(ss, name, ',');
            std::getline(ss, category, ',');
            ss >> hierarchyLevel;
            ss.ignore(); // Skip comma
            std::getline(ss, parent, ',');
            ss >> totalTime;
            ss.ignore();
            ss >> avgTime;
            
            // Store baseline average time
            baselineTimes[name] = avgTime;
        }
        
        // Compare current metrics with baseline
        int regressionCount = 0;
        
        for (const auto& pair : m_metrics) {
            const auto& metrics = pair.second;
            auto it = baselineTimes.find(metrics.name);
            
            if (it != baselineTimes.end()) {
                double baselineTime = it->second;
                double currentTime = metrics.averageTimePerCall;
                
                // Check for regression
                if (currentTime > baselineTime * (1.0 + thresholdPercent / 100.0)) {
                    regressionCount++;
                    std::cout << "Regression detected in '" << metrics.name 
                              << "': baseline=" << baselineTime << "ms, current=" 
                              << currentTime << "ms (+" 
                              << ((currentTime - baselineTime) / baselineTime * 100.0) 
                              << "%)" << std::endl;
                }
            }
        }
        
        return regressionCount;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during regression detection: " << e.what() << std::endl;
        return -1;
    }
}

bool ProfilingManager::saveAsBaseline(const std::string& filename) const {
    return exportMetricsToCSV(filename);
}

void ProfilingManager::trackAllocation(size_t bytes) {
    if (!m_enabled || m_scopeStack.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    std::string currentScope = m_scopeStack.top();
    m_scopeAllocations[currentScope] += bytes;
}

void ProfilingManager::trackDeallocation(size_t bytes) {
    if (!m_enabled || m_scopeStack.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    std::string currentScope = m_scopeStack.top();
    m_scopeDeallocations[currentScope] += bytes;
}

void ProfilingManager::checkPerformanceBudget(const std::string& name, double timeMs) {
    auto it = m_metrics.find(name);
    if (it == m_metrics.end()) {
        return;
    }
    
    auto& metrics = it->second;
    
    if (metrics.budgetMs > 0.0 && timeMs > metrics.budgetMs) {
        metrics.budgetExceededCount++;
        
        if (m_budgetExceededCallback) {
            m_budgetExceededCallback(name, timeMs, metrics.budgetMs);
        }
    }
}

} // namespace Performance