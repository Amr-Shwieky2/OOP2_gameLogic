#include "Memory/MemoryPoolBase.h"
#include <cstring>

MemoryPoolBase::MemoryPoolBase(size_t blockSize, size_t initialCapacity)
    : m_blockSize(std::max(blockSize, sizeof(void*)))  // Block size must be at least large enough to hold a pointer
{
    // Create initial memory chunk
    MemoryChunk initialChunk = createChunk(initialCapacity);
    m_chunks.push_back(std::move(initialChunk));
    
    // Set up free list
    linkFreeBlocks(m_chunks.back());
}

MemoryPoolBase::~MemoryPoolBase() {
    // Memory chunks will be automatically freed by std::vector
}

void* MemoryPoolBase::allocate() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // If free list is empty, expand the pool
    if (!m_freeList) {
        size_t newBlockCount = m_chunks.empty() ? 32 : m_chunks.back().blockCapacity * 2;
        expand(newBlockCount);
        
        // If still no free blocks, allocation failed
        if (!m_freeList) {
            std::cerr << "Memory pool allocation failed. Could not expand pool." << std::endl;
            return nullptr;
        }
    }
    
    // Get a block from the free list
    void* allocatedBlock = m_freeList;
    
    // Update free list to point to the next free block
    m_freeList = *static_cast<void**>(allocatedBlock);
    
    // Clear the memory to avoid any old data
    std::memset(allocatedBlock, 0, m_blockSize);
    
    // Update stats
    m_allocatedBlocks++;
    
    return allocatedBlock;
}

void MemoryPoolBase::deallocate(void* ptr) {
    if (!ptr) return;
    
    // Check if the pointer belongs to this pool
    if (!ownsPointer(ptr)) {
        std::cerr << "Warning: Attempted to deallocate a pointer not owned by this pool." << std::endl;
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Add the block back to the free list
    *static_cast<void**>(ptr) = m_freeList;
    m_freeList = ptr;
    
    // Update stats
    m_allocatedBlocks--;
}

void MemoryPoolBase::expand(size_t additionalBlocks) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Create a new chunk with the requested number of blocks
    MemoryChunk newChunk = createChunk(additionalBlocks);
    
    // Link the new blocks and add to free list
    linkFreeBlocks(newChunk);
    
    // Store the chunk
    m_chunks.push_back(std::move(newChunk));
    
    // Update expand count for stats
    m_expandCount++;
}

void MemoryPoolBase::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Reset free list
    m_freeList = nullptr;
    
    // Relink all blocks in all chunks
    for (auto& chunk : m_chunks) {
        linkFreeBlocks(chunk);
    }
    
    // Reset stats
    m_allocatedBlocks = 0;
}

MemoryPoolBase::MemoryChunk MemoryPoolBase::createChunk(size_t numBlocks) {
    MemoryChunk chunk;
    chunk.blockSize = m_blockSize;
    chunk.blockCapacity = numBlocks;
    
    // Allocate memory for the chunk
    chunk.memory.resize(numBlocks * m_blockSize, 0);
    
    return chunk;
}

void MemoryPoolBase::linkFreeBlocks(MemoryChunk& chunk) {
    // Get the base pointer of the chunk
    uint8_t* basePtr = chunk.memory.data();
    
    // Iterate through blocks, linking each to the next
    for (size_t i = 0; i < chunk.blockCapacity - 1; i++) {
        void* currentBlock = static_cast<void*>(basePtr + (i * m_blockSize));
        void* nextBlock = static_cast<void*>(basePtr + ((i + 1) * m_blockSize));
        
        // Store the pointer to the next block
        *static_cast<void**>(currentBlock) = nextBlock;
    }
    
    // Set the last block to point to the current free list
    void* lastBlock = static_cast<void*>(basePtr + ((chunk.blockCapacity - 1) * m_blockSize));
    *static_cast<void**>(lastBlock) = m_freeList;
    
    // Update the free list to point to the first block of the new chunk
    m_freeList = static_cast<void*>(basePtr);
}

bool MemoryPoolBase::ownsPointer(const void* ptr) const {
    if (!ptr) return false;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if the pointer is within any of our chunks
    for (const auto& chunk : m_chunks) {
        const uint8_t* start = chunk.memory.data();
        const uint8_t* end = start + (chunk.blockCapacity * m_blockSize);
        
        if (static_cast<const uint8_t*>(ptr) >= start && 
            static_cast<const uint8_t*>(ptr) < end) {
            
            // Check if the pointer is block-aligned
            size_t offset = static_cast<const uint8_t*>(ptr) - start;
            return (offset % m_blockSize) == 0;
        }
    }
    
    return false;
}

MemoryPoolBase::MemoryChunk* MemoryPoolBase::findChunkForPointer(const void* ptr) {
    for (auto& chunk : m_chunks) {
        const uint8_t* start = chunk.memory.data();
        const uint8_t* end = start + (chunk.blockCapacity * m_blockSize);
        
        if (static_cast<const uint8_t*>(ptr) >= start && 
            static_cast<const uint8_t*>(ptr) < end) {
            return &chunk;
        }
    }
    
    return nullptr;
}

size_t MemoryPoolBase::getTotalMemoryUsage() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t total = 0;
    for (const auto& chunk : m_chunks) {
        total += chunk.memory.size();
    }
    
    return total;
}

MemoryPoolBase::Stats MemoryPoolBase::getStats() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    Stats stats;
    stats.blockSize = m_blockSize;
    stats.allocatedBlocks = m_allocatedBlocks;
    stats.expandCount = m_expandCount;
    
    stats.totalBlocks = 0;
    for (const auto& chunk : m_chunks) {
        stats.totalBlocks += chunk.blockCapacity;
    }
    
    stats.freeBlocks = stats.totalBlocks - stats.allocatedBlocks;
    stats.capacityBytes = stats.totalBlocks * m_blockSize;
    stats.usedBytes = stats.allocatedBlocks * m_blockSize;
    
    return stats;
}

void MemoryPoolBase::printStats() const {
    Stats stats = getStats();
    
    std::cout << "=== Memory Pool Statistics ===" << std::endl;
    std::cout << "Block size: " << stats.blockSize << " bytes" << std::endl;
    std::cout << "Total blocks: " << stats.totalBlocks << std::endl;
    std::cout << "Allocated blocks: " << stats.allocatedBlocks << std::endl;
    std::cout << "Free blocks: " << stats.freeBlocks << std::endl;
    std::cout << "Total memory capacity: " << stats.capacityBytes << " bytes" << std::endl;
    std::cout << "Memory in use: " << stats.usedBytes << " bytes" << std::endl;
    std::cout << "Memory utilization: " << 
        (stats.totalBlocks > 0 ? (100.0f * stats.allocatedBlocks / stats.totalBlocks) : 0.0f) << "%" << std::endl;
    std::cout << "Expand count: " << stats.expandCount << std::endl;
    std::cout << "===========================" << std::endl;
}