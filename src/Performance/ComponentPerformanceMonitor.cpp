#include "Performance/ComponentPerformanceMonitor.h"
#include <sstream>
#include <algorithm>
#include <vector>
#include <iomanip>

namespace Performance {

ComponentPerformanceMonitor& ComponentPerformanceMonitor::getInstance() {
    static ComponentPerformanceMonitor instance;
    return instance;
}

ComponentPerformanceMonitor::ComponentPerformanceMonitor() {
    // Register budget exceeded callback
    m_budgetExceededCallback = [](const std::string& name, double actual, double budget) {
        std::cerr << "[ComponentPerformanceMonitor] Component operation '"
                  << name << "' exceeded budget: " << actual << "ms (budget: " 
                  << budget << "ms)" << std::endl;
    };
    
    ProfilingManager::getInstance().setBudgetExceededCallback(m_budgetExceededCallback);
}

std::string ComponentPerformanceMonitor::generateComponentReport() const {
    std::stringstream report;
    
    // Get all metrics from the profiling manager
    auto metrics = ProfilingManager::getInstance().getAllMetrics();
    
    // Filter only component metrics
    std::vector<std::reference_wrapper<const ProfileMetrics>> componentMetrics;
    for (const auto& metric : metrics) {
        if (metric.category == "Component") {
            componentMetrics.push_back(std::cref(metric));
        }
    }
    
    // Sort by total time
    std::sort(componentMetrics.begin(), componentMetrics.end(),
              [](const ProfileMetrics& a, const ProfileMetrics& b) {
                  return a.totalTime > b.totalTime;
              });
    
    // Generate report
    report << "===== Component Performance Report =====" << std::endl;
    report << componentMetrics.size() << " component operations tracked" << std::endl << std::endl;
    
    // Table headers
    report << std::left << std::setw(40) << "Component Operation";
    report << std::right << std::setw(12) << "Total (ms)";
    report << std::right << std::setw(12) << "Avg (ms)";
    report << std::right << std::setw(12) << "Calls";
    report << std::right << std::setw(15) << "Memory Delta";
    report << std::right << std::setw(15) << "Budget";
    report << std::right << std::setw(15) << "Exceeded";
    report << std::endl;
    
    report << std::string(111, '-') << std::endl;
    
    // Table data
    for (const auto& metricRef : componentMetrics) {
        const auto& metric = metricRef.get();
        
        double totalTimeMs = metric.totalTime.count() / 1000000.0;
        
        report << std::left << std::setw(40) << metric.name;
        report << std::right << std::setw(12) << std::fixed << std::setprecision(3) << totalTimeMs;
        report << std::right << std::setw(12) << std::fixed << std::setprecision(3) << metric.averageTimePerCall;
        report << std::right << std::setw(12) << metric.callCount;
        report << std::right << std::setw(15) << metric.netMemoryChange;
        report << std::right << std::setw(15) << std::fixed << std::setprecision(2) << metric.budgetMs;
        report << std::right << std::setw(15) << metric.budgetExceededCount;
        report << std::endl;
    }
    
    report << "=====================================" << std::endl;
    
    return report.str();
}

} // namespace Performance