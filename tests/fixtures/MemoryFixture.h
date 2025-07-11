#pragma once
#include <gtest/gtest.h>
#include "Memory/MemoryManager.h"
#include "Memory/MemoryLeakDetector.h"

/**
 * Fixture for memory-related tests
 * Enables memory leak detection during tests
 */
class MemoryFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Enable memory leak detection
        MemoryLeakDetector::getInstance().setEnabled(true);
        MemoryLeakDetector::getInstance().resetCounters();
        
        // Record initial memory usage
        m_initialMemoryUsage = MemoryManager::getInstance().getTotalMemoryUsage();
        m_initialAllocationCount = MemoryLeakDetector::getInstance().getAllocationCount();
    }
    
    void TearDown() override {
        // Check for memory leaks
        size_t currentMemoryUsage = MemoryManager::getInstance().getTotalMemoryUsage();
        size_t currentAllocationCount = MemoryLeakDetector::getInstance().getAllocationCount();
        
        // Only disable leak detection after checking
        MemoryLeakDetector::getInstance().setEnabled(false);
        
        // If memory usage has increased, we may have a leak
        EXPECT_EQ(m_initialMemoryUsage, currentMemoryUsage)
            << "Possible memory leak detected. Memory usage increased by "
            << (currentMemoryUsage - m_initialMemoryUsage) << " bytes";
            
        // If allocation count doesn't match, we have a leak
        EXPECT_EQ(m_initialAllocationCount, currentAllocationCount)
            << "Memory leak detected. " 
            << (currentAllocationCount - m_initialAllocationCount)
            << " allocations were not freed";
    }
    
    // Initial memory state
    size_t m_initialMemoryUsage = 0;
    size_t m_initialAllocationCount = 0;
};