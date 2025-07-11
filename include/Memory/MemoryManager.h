#pragma once
#include "MemoryPoolBase.h"
#include "ObjectPool.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>

/**
 * MemoryManager - Global memory management and profiling system
 * 
 * This singleton class manages memory pools for different object types
 * and provides memory usage tracking and reporting.
 */
class MemoryManager {
public:
    /**
     * Get the singleton instance
     * 
     * @return Reference to the MemoryManager instance
     */
    static MemoryManager& getInstance();

    /**
     * Get a memory pool for a specific type
     * 
     * @tparam T The type of objects to allocate from the pool
     * @tparam BlockSize Size of each memory block (defaults to sizeof(T))
     * @tparam InitialCapacity Initial capacity of the pool
     * @return Reference to the object pool
     */
    template<typename T, size_t BlockSize = sizeof(T), size_t InitialCapacity = 32>
    ObjectPool<T, BlockSize, InitialCapacity>& getPool() {
        std::type_index typeIdx(typeid(T));
        std::string typeName = typeid(T).name();
        
        // Check if this pool already exists
        auto it = m_pools.find(typeIdx);
        if (it != m_pools.end()) {
            // Pool exists, return it as the correct type
            return *static_cast<ObjectPool<T, BlockSize, InitialCapacity>*>(it->second.get());
        }
        
        // Create a new pool for this type
        auto pool = std::make_unique<ObjectPool<T, BlockSize, InitialCapacity>>();
        
        // Register pool information for profiling
        m_typeNames[typeIdx] = typeName;
        
        // Store and return the pool
        auto* poolPtr = pool.get();
        m_pools[typeIdx] = std::move(pool);
        return *poolPtr;
    }
    
    /**
     * Record a memory allocation
     * 
     * @param size Size of the allocation in bytes
     * @param typeName Type name for the allocation
     */
    void recordAllocation(size_t size, const std::string& typeName);
    
    /**
     * Record a memory deallocation
     * 
     * @param size Size of the deallocation in bytes
     * @param typeName Type name for the deallocation
     */
    void recordDeallocation(size_t size, const std::string& typeName);
    
    /**
     * Start tracking a memory operation for profiling
     * 
     * @param operationName Name of the operation
     * @return Unique ID for the operation
     */
    size_t startOperation(const std::string& operationName);
    
    /**
     * End tracking a memory operation
     * 
     * @param operationId ID of the operation to end
     */
    void endOperation(size_t operationId);
    
    /**
     * Print memory usage statistics
     */
    void printMemoryUsage() const;
    
    /**
     * Export memory usage report to a CSV file
     * 
     * @param filename The filename for the CSV report
     * @return true if the export was successful
     */
    bool exportMemoryReport(const std::string& filename) const;
    
    /**
     * Set debug mode (more verbose output)
     * 
     * @param enabled Whether debug mode should be enabled
     */
    void setDebugMode(bool enabled) { m_debugMode = enabled; }
    
    /**
     * Get current memory usage in bytes
     * 
     * @return Current memory usage
     */
    size_t getCurrentMemoryUsage() const { return m_currentMemoryUsage; }
    
    /**
     * Get peak memory usage in bytes
     * 
     * @return Peak memory usage
     */
    size_t getPeakMemoryUsage() const { return m_peakMemoryUsage; }
    
    /**
     * Get total allocations count
     * 
     * @return Total number of allocations
     */
    size_t getAllocationCount() const { return m_allocationCount; }
    
    /**
     * Reset memory statistics
     */
    void resetStats();

private:
    // Private constructor for singleton
    MemoryManager();
    ~MemoryManager() = default;

    // Map of type index to object pool
    std::unordered_map<std::type_index, std::unique_ptr<void, std::function<void(void*)>>> m_pools;
    
    // Map of type index to type name (for reporting)
    std::unordered_map<std::type_index, std::string> m_typeNames;
    
    // Memory usage tracking
    std::atomic<size_t> m_currentMemoryUsage{0};
    std::atomic<size_t> m_peakMemoryUsage{0};
    std::atomic<size_t> m_allocationCount{0};
    std::atomic<size_t> m_deallocationCount{0};
    
    // Type-specific memory usage tracking
    struct TypeStats {
        size_t currentBytes = 0;
        size_t peakBytes = 0;
        size_t allocations = 0;
        size_t deallocations = 0;
    };
    
    mutable std::mutex m_statsMutex;
    std::unordered_map<std::string, TypeStats> m_typeStats;
    
    // Operation profiling
    struct OperationRecord {
        std::string name;
        std::chrono::steady_clock::time_point startTime;
        bool active = true;
    };
    
    std::vector<OperationRecord> m_operations;
    std::unordered_map<std::string, double> m_operationTimes; // in milliseconds
    
    // Debug mode flag
    bool m_debugMode = false;
    
    // Next operation ID
    std::atomic<size_t> m_nextOperationId{0};
};