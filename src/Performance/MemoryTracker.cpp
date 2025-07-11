#include "Performance/MemoryTracker.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

namespace Performance {

// Static member initialization
std::mutex MemoryTracker::s_mutex;
std::unordered_map<void*, std::pair<size_t, std::string>> MemoryTracker::s_allocations;
std::unordered_map<std::string, size_t> MemoryTracker::s_typeAllocationCount;
std::unordered_map<std::string, size_t> MemoryTracker::s_typeMemoryUsage;
size_t MemoryTracker::s_totalAllocated = 0;
size_t MemoryTracker::s_peakMemory = 0;

void MemoryTracker::trackAllocation(void* ptr, size_t size, const std::string& typeName) {
    if (!ptr || size == 0) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(s_mutex);
    
    // Track this allocation
    s_allocations[ptr] = std::make_pair(size, typeName);
    
    // Update type statistics
    s_typeAllocationCount[typeName]++;
    s_typeMemoryUsage[typeName] += size;
    
    // Update global statistics
    s_totalAllocated += size;
    s_peakMemory = std::max(s_peakMemory, s_totalAllocated);
}

void MemoryTracker::trackDeallocation(void* ptr) {
    if (!ptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(s_mutex);
    
    auto it = s_allocations.find(ptr);
    if (it != s_allocations.end()) {
        size_t size = it->second.first;
        const std::string& typeName = it->second.second;
        
        // Update type statistics
        if (s_typeMemoryUsage[typeName] >= size) {
            s_typeMemoryUsage[typeName] -= size;
        }
        
        // Update global statistics
        if (s_totalAllocated >= size) {
            s_totalAllocated -= size;
        }
        
        // Remove allocation
        s_allocations.erase(it);
    }
}

size_t MemoryTracker::getTotalAllocated() {
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_totalAllocated;
}

size_t MemoryTracker::getPeakMemory() {
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_peakMemory;
}

size_t MemoryTracker::getTypeAllocationCount(const std::string& typeName) {
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_typeAllocationCount[typeName];
}

size_t MemoryTracker::getTypeMemoryUsage(const std::string& typeName) {
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_typeMemoryUsage[typeName];
}

void MemoryTracker::reset() {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    s_allocations.clear();
    s_typeAllocationCount.clear();
    s_typeMemoryUsage.clear();
    s_totalAllocated = 0;
    s_peakMemory = 0;
    
    std::cout << "[MemoryTracker] Reset" << std::endl;
}

void MemoryTracker::printStats() {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    std::cout << "===== Memory Allocation Statistics =====" << std::endl;
    std::cout << "Current memory: " << s_totalAllocated << " bytes" << std::endl;
    std::cout << "Peak memory: " << s_peakMemory << " bytes" << std::endl;
    std::cout << "Active allocations: " << s_allocations.size() << std::endl;
    
    // Sort types by memory usage
    using TypeUsagePair = std::pair<std::string, size_t>;
    std::vector<TypeUsagePair> sortedTypes;
    
    for (const auto& pair : s_typeMemoryUsage) {
        if (pair.second > 0) {
            sortedTypes.push_back(pair);
        }
    }
    
    std::sort(sortedTypes.begin(), sortedTypes.end(),
              [](const TypeUsagePair& a, const TypeUsagePair& b) {
                  return a.second > b.second;
              });
    
    if (!sortedTypes.empty()) {
        std::cout << "\n--- Memory Usage by Type ---" << std::endl;
        std::cout << std::left << std::setw(40) << "Type"
                  << std::right << std::setw(15) << "Memory (bytes)"
                  << std::setw(15) << "Allocations"
                  << std::setw(10) << "%" << std::endl;
        
        for (const auto& pair : sortedTypes) {
            float percentage = 100.0f * static_cast<float>(pair.second) / s_totalAllocated;
            std::cout << std::left << std::setw(40) << pair.first
                      << std::right << std::setw(15) << pair.second
                      << std::setw(15) << s_typeAllocationCount[pair.first]
                      << std::setw(9) << std::fixed << std::setprecision(1) << percentage << "%" << std::endl;
        }
    }
    
    std::cout << "=================================" << std::endl;
}

// ScopedMemoryTracker implementation
ScopedMemoryTracker::ScopedMemoryTracker(const std::string& scopeName)
    : m_scopeName(scopeName), m_startTotal(MemoryTracker::getTotalAllocated())
{
}

ScopedMemoryTracker::~ScopedMemoryTracker() {
    if (m_active) {
        size_t endTotal = MemoryTracker::getTotalAllocated();
        
        if (endTotal > m_startTotal) {
            m_allocated = endTotal - m_startTotal;
        }
        else if (endTotal < m_startTotal) {
            m_deallocated = m_startTotal - endTotal;
        }
        
        std::cout << "[MemoryTracker] Scope '" << m_scopeName
                  << "': Allocated: " << m_allocated
                  << " bytes, Deallocated: " << m_deallocated
                  << " bytes, Net: ";
        
        int64_t netChange = getNetChange();
        if (netChange > 0) {
            std::cout << "+" << netChange << " bytes" << std::endl;
        }
        else {
            std::cout << netChange << " bytes" << std::endl;
        }
    }
}

} // namespace Performance