#pragma once
#include "MemoryPoolBase.h"
#include <type_traits>
#include <stdexcept>
#include <memory>

/**
 * ObjectPool - A type-specific memory pool for efficient object allocation
 * 
 * This template class provides a type-safe interface for allocating objects of type T
 * from a memory pool. It handles proper construction and destruction of objects,
 * while delegating the raw memory management to the MemoryPoolBase class.
 * 
 * @tparam T The type of objects to allocate
 * @tparam BlockSize Optional parameter to override the block size (must be >= sizeof(T))
 * @tparam InitialCapacity The initial number of objects the pool can hold
 * @tparam ExpandFactor The factor by which the pool expands when full
 */
template<typename T, size_t BlockSize = sizeof(T), size_t InitialCapacity = 32>
class ObjectPool {
    // Ensure the block size is adequate for the object
    static_assert(BlockSize >= sizeof(T), "Block size must be at least as large as the size of T");
    // Ensure T is not a reference or const/volatile qualified
    static_assert(!std::is_reference<T>::value && !std::is_const<T>::value && !std::is_volatile<T>::value,
                 "T cannot be reference, const, or volatile");

public:
    /**
     * Constructor - initializes a memory pool for objects of type T
     */
    ObjectPool() : m_pool(BlockSize, InitialCapacity) {}
    
    /**
     * Destructor - destroys the pool, all allocated objects must have been 
     * returned to the pool already
     */
    ~ObjectPool() = default;
    
    /**
     * Create a new object of type T with given constructor arguments
     * 
     * @param args Constructor arguments for T
     * @return A pointer to the newly created object
     */
    template<typename... Args>
    T* create(Args&&... args) {
        void* memory = m_pool.allocate();
        if (!memory) {
            return nullptr;
        }
        
        // Construct the object in-place
        try {
            return new(memory) T(std::forward<Args>(args)...);
        }
        catch (...) {
            // If construction throws, free the memory and rethrow
            m_pool.deallocate(memory);
            throw;
        }
    }
    
    /**
     * Return an object to the pool, calling its destructor
     * 
     * @param obj The object to return to the pool
     */
    void destroy(T* obj) {
        if (!obj) return;
        
        // Ensure the object belongs to this pool
        if (!m_pool.ownsPointer(obj)) {
            throw std::invalid_argument("Object does not belong to this pool");
        }
        
        // Call the destructor
        obj->~T();
        
        // Return memory to the pool
        m_pool.deallocate(obj);
    }
    
    /**
     * Expand the pool by adding more capacity
     * 
     * @param additionalCapacity Number of additional objects to allocate space for
     */
    void expand(size_t additionalCapacity) {
        m_pool.expand(additionalCapacity);
    }
    
    /**
     * Reset the pool, invalidating all previously allocated objects
     * WARNING: This will cause memory leaks if objects have not been properly destroyed
     */
    void reset() {
        m_pool.reset();
    }
    
    /**
     * Get statistics about the pool
     * 
     * @return Stats struct with pool statistics
     */
    typename MemoryPoolBase::Stats getStats() const {
        return m_pool.getStats();
    }
    
    /**
     * Print statistics about the pool to std::cout
     */
    void printStats() const {
        m_pool.printStats();
    }
    
    /**
     * Get the total memory usage of the pool in bytes
     * 
     * @return Memory usage in bytes
     */
    size_t getMemoryUsage() const {
        return m_pool.getTotalMemoryUsage();
    }
    
    /**
     * Check if a pointer was allocated from this pool
     * 
     * @param ptr The pointer to check
     * @return true if the pointer is owned by this pool
     */
    bool ownsPointer(const T* ptr) const {
        return m_pool.ownsPointer(ptr);
    }

private:
    MemoryPoolBase m_pool;
};