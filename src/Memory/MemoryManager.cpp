#include "Memory/MemoryManager.h"
#include <iomanip>
#include <sstream>

MemoryManager& MemoryManager::getInstance() {
    static MemoryManager instance;
    return instance;
}

MemoryManager::MemoryManager() {
    std::cout << "Memory Manager initialized" << std::endl;
}

void MemoryManager::recordAllocation(size_t size, const std::string& typeName) {
    // Update global stats
    m_currentMemoryUsage += size;
    m_peakMemoryUsage = std::max(m_peakMemoryUsage.load(), m_currentMemoryUsage.load());
    m_allocationCount++;
    
    // Update type-specific stats
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        TypeStats& stats = m_typeStats[typeName];
        stats.currentBytes += size;
        stats.peakBytes = std::max(stats.peakBytes, stats.currentBytes);
        stats.allocations++;
    }
    
    // Debug output if enabled
    if (m_debugMode) {
        std::cout << "[MEMORY] Allocated " << size << " bytes for " << typeName
                  << " (Total: " << m_currentMemoryUsage << " bytes)" << std::endl;
    }
}

void MemoryManager::recordDeallocation(size_t size, const std::string& typeName) {
    // Update global stats
    if (m_currentMemoryUsage >= size) {
        m_currentMemoryUsage -= size;
    }
    else {
        // This is a safeguard against potential accounting errors
        m_currentMemoryUsage = 0;
    }
    m_deallocationCount++;
    
    // Update type-specific stats
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        TypeStats& stats = m_typeStats[typeName];
        if (stats.currentBytes >= size) {
            stats.currentBytes -= size;
        }
        else {
            // Safeguard against potential accounting errors
            stats.currentBytes = 0;
        }
        stats.deallocations++;
    }
    
    // Debug output if enabled
    if (m_debugMode) {
        std::cout << "[MEMORY] Deallocated " << size << " bytes for " << typeName
                  << " (Total: " << m_currentMemoryUsage << " bytes)" << std::endl;
    }
}

size_t MemoryManager::startOperation(const std::string& operationName) {
    size_t operationId = m_nextOperationId++;
    
    // Add the operation record
    if (operationId >= m_operations.size()) {
        m_operations.resize(operationId + 1);
    }
    
    m_operations[operationId] = {
        operationName,
        std::chrono::steady_clock::now(),
        true
    };
    
    return operationId;
}

void MemoryManager::endOperation(size_t operationId) {
    if (operationId >= m_operations.size() || !m_operations[operationId].active) {
        return; // Invalid operation ID or already ended
    }
    
    auto& operation = m_operations[operationId];
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        endTime - operation.startTime).count() / 1000.0; // Convert to milliseconds
    
    // Mark as inactive
    operation.active = false;
    
    // Update the operation time
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        m_operationTimes[operation.name] += duration;
    }
    
    // Debug output if enabled
    if (m_debugMode) {
        std::cout << "[MEMORY] Operation '" << operation.name
                  << "' completed in " << duration << " ms" << std::endl;
    }
}

void MemoryManager::printMemoryUsage() const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    std::cout << "===== Memory Usage Statistics =====" << std::endl;
    std::cout << "Current memory usage: " << m_currentMemoryUsage << " bytes" << std::endl;
    std::cout << "Peak memory usage: " << m_peakMemoryUsage << " bytes" << std::endl;
    std::cout << "Total allocations: " << m_allocationCount << std::endl;
    std::cout << "Total deallocations: " << m_deallocationCount << std::endl;
    
    if (m_allocationCount > m_deallocationCount) {
        std::cout << "WARNING: " << (m_allocationCount - m_deallocationCount)
                  << " allocations have not been deallocated!" << std::endl;
    }
    
    std::cout << "\n--- Memory Usage by Type ---" << std::endl;
    std::cout << std::left << std::setw(30) << "Type"
              << std::right << std::setw(15) << "Current" 
              << std::setw(15) << "Peak"
              << std::setw(15) << "Allocations"
              << std::setw(15) << "Deallocations"
              << std::endl;
    
    for (const auto& [typeName, stats] : m_typeStats) {
        std::cout << std::left << std::setw(30) << typeName
                  << std::right << std::setw(15) << stats.currentBytes
                  << std::setw(15) << stats.peakBytes
                  << std::setw(15) << stats.allocations
                  << std::setw(15) << stats.deallocations
                  << std::endl;
    }
    
    std::cout << "\n--- Operation Timings ---" << std::endl;
    std::cout << std::left << std::setw(30) << "Operation"
              << std::right << std::setw(15) << "Time (ms)"
              << std::endl;
    
    for (const auto& [opName, time] : m_operationTimes) {
        std::cout << std::left << std::setw(30) << opName
                  << std::right << std::setw(15) << std::fixed << std::setprecision(2) << time
                  << std::endl;
    }
    
    std::cout << "=================================" << std::endl;
    
    // Print pool statistics
    std::cout << "\n===== Memory Pool Statistics =====" << std::endl;
    for (const auto& [typeIdx, poolPtr] : m_pools) {
        std::cout << "Pool for type: " << m_typeNames.at(typeIdx) << std::endl;
        // We can't directly access the pool's printStats() method due to type erasure
        // This is handled by the specific pool methods when used directly
    }
}

bool MemoryManager::exportMemoryReport(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        
        // Write header
        file << "Memory Usage Report" << std::endl;
        file << "Generated: " << std::time(nullptr) << std::endl;
        file << std::endl;
        
        // Write global stats
        file << "Global Statistics" << std::endl;
        file << "Current Memory Usage (bytes)," << m_currentMemoryUsage << std::endl;
        file << "Peak Memory Usage (bytes)," << m_peakMemoryUsage << std::endl;
        file << "Total Allocations," << m_allocationCount << std::endl;
        file << "Total Deallocations," << m_deallocationCount << std::endl;
        file << std::endl;
        
        // Write type stats
        file << "Type Statistics" << std::endl;
        file << "Type,Current Bytes,Peak Bytes,Allocations,Deallocations" << std::endl;
        
        for (const auto& [typeName, stats] : m_typeStats) {
            file << typeName << ","
                 << stats.currentBytes << ","
                 << stats.peakBytes << ","
                 << stats.allocations << ","
                 << stats.deallocations << std::endl;
        }
        
        file << std::endl;
        
        // Write operation timings
        file << "Operation Timings" << std::endl;
        file << "Operation,Time (ms)" << std::endl;
        
        for (const auto& [opName, time] : m_operationTimes) {
            file << opName << "," << time << std::endl;
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during memory report export: " << e.what() << std::endl;
        return false;
    }
}

void MemoryManager::resetStats() {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    m_currentMemoryUsage = 0;
    m_peakMemoryUsage = 0;
    m_allocationCount = 0;
    m_deallocationCount = 0;
    
    m_typeStats.clear();
    m_operationTimes.clear();
    
    std::cout << "[MEMORY] Statistics reset" << std::endl;
}