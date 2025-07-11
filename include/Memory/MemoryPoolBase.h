#pragma once
#include <cstdint>
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <atomic>

/**
 * MemoryPoolBase - Base class for memory pool allocators
 * 
 * Provides core memory management functionality for efficient allocation and deallocation
 * of fixed-size memory blocks. This class handles the low-level memory operations,
 * while derived classes provide type-specific interfaces.
 */
class MemoryPoolBase {
public:
    // Constructor - initializes a pool with a given block size and initial capacity
    MemoryPoolBase(size_t blockSize, size_t initialCapacity = 32);
    
    // Destructor - frees all allocated memory
    virtual ~MemoryPoolBase();
    
    // Allocate a block of memory
    void* allocate();
    
    // Free a previously allocated block
    void deallocate(void* ptr);
    
    // Expand the pool by adding more blocks
    void expand(size_t additionalBlocks);
    
    // Reset the pool, invalidating all previously allocated blocks
    void reset();
    
    // Get total memory usage in bytes
    size_t getTotalMemoryUsage() const;
    
    // Get statistics about the pool
    struct Stats {
        size_t blockSize;
        size_t totalBlocks;
        size_t allocatedBlocks;
        size_t freeBlocks;
        size_t capacityBytes;
        size_t usedBytes;
        size_t expandCount;
    };
    
    Stats getStats() const;
    
    // Print statistics to std::cout
    void printStats() const;
    
    // Check if a pointer was allocated from this pool
    bool ownsPointer(const void* ptr) const;
    
    // Get the block size
    size_t getBlockSize() const { return m_blockSize; }
    
protected:
    // Memory block structure
    struct MemoryChunk {
        std::vector<uint8_t> memory;
        size_t blockSize;
        size_t blockCapacity;
    };
    
    // Initialize a new memory chunk
    MemoryChunk createChunk(size_t numBlocks);
    
    // Link free blocks in a chunk
    void linkFreeBlocks(MemoryChunk& chunk);
    
    // Find which chunk a pointer belongs to
    MemoryChunk* findChunkForPointer(const void* ptr);
    
    // Block size in bytes
    size_t m_blockSize;
    
    // Free list pointer
    void* m_freeList = nullptr;
    
    // Memory chunks
    std::vector<MemoryChunk> m_chunks;
    
    // Statistics
    std::atomic<size_t> m_allocatedBlocks{0};
    std::atomic<size_t> m_expandCount{0};
    
    // Thread safety
    mutable std::mutex m_mutex;
};