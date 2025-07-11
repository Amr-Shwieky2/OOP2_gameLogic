#pragma once
#include "Performance/Profiler.h"
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <mutex>
#include <string>

namespace Performance {

/**
 * @brief Static memory usage tracking utilities
 */
class MemoryTracker {
public:
    /**
     * @brief Track an allocation
     * 
     * @param ptr Pointer to allocated memory
     * @param size Size in bytes
     * @param typeName Name of the type being allocated
     */
    static void trackAllocation(void* ptr, size_t size, const std::string& typeName);
    
    /**
     * @brief Track a deallocation
     * 
     * @param ptr Pointer to deallocated memory
     */
    static void trackDeallocation(void* ptr);
    
    /**
     * @brief Get total allocated memory
     * 
     * @return Current allocated memory in bytes
     */
    static size_t getTotalAllocated();
    
    /**
     * @brief Get peak memory usage
     * 
     * @return Peak memory usage in bytes
     */
    static size_t getPeakMemory();
    
    /**
     * @brief Get allocation count for a specific type
     * 
     * @param typeName Name of the type
     * @return Number of allocations for the type
     */
    static size_t getTypeAllocationCount(const std::string& typeName);
    
    /**
     * @brief Get memory usage for a specific type
     * 
     * @param typeName Name of the type
     * @return Memory usage in bytes for the type
     */
    static size_t getTypeMemoryUsage(const std::string& typeName);
    
    /**
     * @brief Reset tracking statistics
     */
    static void reset();
    
    /**
     * @brief Print memory usage statistics
     */
    static void printStats();
    
private:
    // Private constructor to prevent instantiation
    MemoryTracker() = default;
    
    // Thread safety
    static std::mutex s_mutex;
    
    // Allocation tracking
    static std::unordered_map<void*, std::pair<size_t, std::string>> s_allocations;
    static std::unordered_map<std::string, size_t> s_typeAllocationCount;
    static std::unordered_map<std::string, size_t> s_typeMemoryUsage;
    
    // Global statistics
    static size_t s_totalAllocated;
    static size_t s_peakMemory;
};

/**
 * @brief Custom allocator that tracks memory usage
 * 
 * @tparam T Type being allocated
 */
template<typename T>
class TrackedAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    // Default constructor
    TrackedAllocator() noexcept = default;
    
    // Copy constructor
    template<typename U>
    TrackedAllocator(const TrackedAllocator<U>&) noexcept {}
    
    // Rebind allocator to another type
    template<typename U>
    struct rebind {
        using other = TrackedAllocator<U>;
    };
    
    /**
     * @brief Allocate memory for n objects
     * 
     * @param n Number of objects
     * @return Pointer to allocated memory
     */
    pointer allocate(size_type n) {
        size_type bytes = n * sizeof(T);
        pointer p = static_cast<pointer>(::operator new(bytes));
        
        // Track allocation
        MemoryTracker::trackAllocation(p, bytes, typeid(T).name());
        
        // Also track in current profiling scope
        ScopedProfiler* currentProfiler = getCurrentProfiler();
        if (currentProfiler) {
            currentProfiler->trackAllocation(bytes);
        }
        
        return p;
    }
    
    /**
     * @brief Deallocate memory
     * 
     * @param p Pointer to memory
     * @param n Number of objects (not used)
     */
    void deallocate(pointer p, size_type) noexcept {
        if (!p) return;
        
        // Track deallocation
        MemoryTracker::trackDeallocation(p);
        
        // Also track in current profiling scope
        ScopedProfiler* currentProfiler = getCurrentProfiler();
        if (currentProfiler) {
            size_t bytes = sizeof(T); // This is approximate since we don't know n here
            currentProfiler->trackDeallocation(bytes);
        }
        
        ::operator delete(p);
    }
    
    /**
     * @brief Compare for equality with another allocator
     */
    template<typename U>
    bool operator==(const TrackedAllocator<U>&) const noexcept {
        return true;
    }
    
    /**
     * @brief Compare for inequality with another allocator
     */
    template<typename U>
    bool operator!=(const TrackedAllocator<U>&) const noexcept {
        return false;
    }
    
private:
    // Helper to get current profiler (if any)
    ScopedProfiler* getCurrentProfiler() {
        // This would need to be implemented to access the current profiler
        // For now, return nullptr
        return nullptr;
    }
};

/**
 * @brief Macro for creating a tracked container
 * 
 * Example: TRACKED_VECTOR(int) myVector;
 */
#define TRACKED_VECTOR(type) std::vector<type, Performance::TrackedAllocator<type>>

/**
 * @brief Macro for creating a tracked map
 * 
 * Example: TRACKED_MAP(int, float) myMap;
 */
#define TRACKED_MAP(key_type, value_type) std::unordered_map<key_type, value_type, \
                                            std::hash<key_type>, \
                                            std::equal_to<key_type>, \
                                            Performance::TrackedAllocator<std::pair<const key_type, value_type>>>

/**
 * @brief Macro for creating a tracked string
 */
#define TRACKED_STRING std::basic_string<char, std::char_traits<char>, Performance::TrackedAllocator<char>>

/**
 * @brief RAII class for tracking memory allocations in a scope
 */
class ScopedMemoryTracker {
public:
    /**
     * @brief Constructor that starts tracking memory
     * @param scopeName Name of the scope
     */
    explicit ScopedMemoryTracker(const std::string& scopeName);
    
    /**
     * @brief Destructor that reports memory usage
     */
    ~ScopedMemoryTracker();
    
    /**
     * @brief Get memory allocated in this scope
     * @return Bytes allocated
     */
    size_t getAllocated() const { return m_allocated; }
    
    /**
     * @brief Get memory deallocated in this scope
     * @return Bytes deallocated
     */
    size_t getDeallocated() const { return m_deallocated; }
    
    /**
     * @brief Get net memory change
     * @return Net bytes allocated or deallocated
     */
    int64_t getNetChange() const { return static_cast<int64_t>(m_allocated) - static_cast<int64_t>(m_deallocated); }
    
private:
    std::string m_scopeName;
    size_t m_startTotal;
    size_t m_allocated{0};
    size_t m_deallocated{0};
    bool m_active{true};
};

// Convenience macro for memory tracking
#define TRACK_MEMORY_SCOPE(name) Performance::ScopedMemoryTracker __memTracker##__LINE__(name)

} // namespace Performance