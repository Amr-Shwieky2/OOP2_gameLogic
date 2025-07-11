#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <functional>
#include <typeinfo>
#include <typeindex>

/**
 * MemoryLeakDetector - Debug tool for tracking memory allocations and detecting leaks
 * 
 * This class is used in debug builds to track all allocations and detect potential
 * memory leaks. It provides detailed information about allocations, including source
 * location, allocation size, and memory addresses.
 */
class MemoryLeakDetector {
public:
    /**
     * Get the singleton instance
     * 
     * @return Reference to the MemoryLeakDetector instance
     */
    static MemoryLeakDetector& getInstance();
    
    /**
     * Track a memory allocation
     * 
     * @param ptr Pointer to allocated memory
     * @param size Size of the allocation
     * @param type Type name of the allocated object
     * @param file Source file where allocation occurred
     * @param line Line number where allocation occurred
     */
    void trackAllocation(void* ptr, size_t size, const std::string& type, 
                         const char* file, int line);
    
    /**
     * Track a memory deallocation
     * 
     * @param ptr Pointer to deallocated memory
     */
    void trackDeallocation(void* ptr);
    
    /**
     * Get current number of tracked allocations
     * 
     * @return Number of allocations currently tracked
     */
    size_t getAllocationCount() const;
    
    /**
     * Check for memory leaks and dump report
     * 
     * @param outputToFile Whether to output leaks to a file
     * @param filename The filename for the leak report
     * @return True if no leaks were detected
     */
    bool checkForLeaks(bool outputToFile = false, const std::string& filename = "memory_leaks.txt");
    
    /**
     * Print a summary of memory usage
     */
    void printMemorySummary() const;
    
    /**
     * Clear all tracked allocations
     */
    void reset();
    
    /**
     * Enable or disable leak detection
     * 
     * @param enabled Whether leak detection should be enabled
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if leak detection is enabled
     * 
     * @return True if leak detection is enabled
     */
    bool isEnabled() const;

private:
    // Private constructor for singleton
    MemoryLeakDetector();
    ~MemoryLeakDetector();
    
    // Struct to hold allocation info
    struct AllocationInfo {
        size_t size;
        std::string type;
        std::string file;
        int line;
        std::string stackTrace;
    };
    
    // Map from pointer to allocation info
    std::unordered_map<void*, AllocationInfo> m_allocations;
    
    // Statistics by type
    std::unordered_map<std::string, size_t> m_typeStats;
    
    // Mutex for thread safety
    mutable std::mutex m_mutex;
    
    // Enabled flag
    bool m_enabled = true;
    
    // Output a list of memory leaks to the given stream
    void dumpLeaks(std::ostream& stream);
    
    // Capture a stack trace for debugging purposes
    std::string captureStackTrace();
};

// Macros for easier leak tracking
#ifdef _DEBUG
    #define TRACK_ALLOC(ptr, size, type) \
        MemoryLeakDetector::getInstance().trackAllocation(ptr, size, type, __FILE__, __LINE__)
        
    #define TRACK_DEALLOC(ptr) \
        MemoryLeakDetector::getInstance().trackDeallocation(ptr)
        
    #define CHECK_FOR_LEAKS() \
        MemoryLeakDetector::getInstance().checkForLeaks()
        
    #define DUMP_LEAKS_TO_FILE(filename) \
        MemoryLeakDetector::getInstance().checkForLeaks(true, filename)
        
    #define PRINT_MEMORY_SUMMARY() \
        MemoryLeakDetector::getInstance().printMemorySummary()
#else
    #define TRACK_ALLOC(ptr, size, type)
    #define TRACK_DEALLOC(ptr)
    #define CHECK_FOR_LEAKS() true
    #define DUMP_LEAKS_TO_FILE(filename) true
    #define PRINT_MEMORY_SUMMARY()
#endif