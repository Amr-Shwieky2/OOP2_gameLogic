#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Memory/MemoryPoolBase.h"
#include "Memory/ObjectPool.h"
#include "Memory/PooledPtr.h"
#include "fixtures/MemoryFixture.h"
#include <string>

class MemoryPoolTest : public MemoryFixture {
protected:
    // Test object class
    class TestObject {
    public:
        TestObject() : m_id(s_nextId++) {}
        explicit TestObject(int value) : m_value(value), m_id(s_nextId++) {}
        ~TestObject() {}
        
        int getValue() const { return m_value; }
        void setValue(int value) { m_value = value; }
        int getId() const { return m_id; }
        
    private:
        int m_value = 0;
        int m_id = 0;
        static int s_nextId;
    };
    
    // Initialize static member
    static int TestObject::s_nextId;
};

int MemoryPoolTest::TestObject::s_nextId = 1;

// Test object pool creation
TEST_F(MemoryPoolTest, PoolCreation) {
    // Create a pool for TestObject instances
    constexpr size_t POOL_SIZE = 10;
    ObjectPool<TestObject> pool(POOL_SIZE);
    
    // Check initial state
    EXPECT_EQ(POOL_SIZE, pool.getCapacity());
    EXPECT_EQ(0, pool.getAllocatedCount());
    EXPECT_EQ(POOL_SIZE, pool.getFreeCount());
}

// Test object allocation
TEST_F(MemoryPoolTest, ObjectAllocation) {
    // Create a pool
    constexpr size_t POOL_SIZE = 10;
    ObjectPool<TestObject> pool(POOL_SIZE);
    
    // Allocate an object
    TestObject* obj = pool.allocate();
    
    // Check that allocation succeeded
    ASSERT_NE(nullptr, obj);
    
    // Check pool stats
    EXPECT_EQ(1, pool.getAllocatedCount());
    EXPECT_EQ(POOL_SIZE - 1, pool.getFreeCount());
    
    // Clean up
    pool.deallocate(obj);
}

// Test object allocation with constructor args
TEST_F(MemoryPoolTest, ObjectAllocationWithArgs) {
    // Create a pool
    constexpr size_t POOL_SIZE = 10;
    ObjectPool<TestObject> pool(POOL_SIZE);
    
    // Allocate with constructor argument
    constexpr int TEST_VALUE = 42;
    TestObject* obj = pool.allocate(TEST_VALUE);
    
    // Check that allocation succeeded and constructor was called
    ASSERT_NE(nullptr, obj);
    EXPECT_EQ(TEST_VALUE, obj->getValue());
    
    // Clean up
    pool.deallocate(obj);
}

// Test pool exhaustion
TEST_F(MemoryPoolTest, PoolExhaustion) {
    // Create a small pool
    constexpr size_t POOL_SIZE = 2;
    ObjectPool<TestObject> pool(POOL_SIZE);
    
    // Allocate all objects
    TestObject* obj1 = pool.allocate();
    TestObject* obj2 = pool.allocate();
    
    // Both allocations should succeed
    ASSERT_NE(nullptr, obj1);
    ASSERT_NE(nullptr, obj2);
    
    // Pool should be full
    EXPECT_EQ(POOL_SIZE, pool.getAllocatedCount());
    EXPECT_EQ(0, pool.getFreeCount());
    
    // Try to allocate one more
    TestObject* obj3 = pool.allocate();
    
    // This should fail
    EXPECT_EQ(nullptr, obj3);
    
    // Clean up
    pool.deallocate(obj1);
    pool.deallocate(obj2);
}

// Test PooledPtr
TEST_F(MemoryPoolTest, PooledPtr) {
    // Create a pool
    constexpr size_t POOL_SIZE = 10;
    ObjectPool<TestObject> pool(POOL_SIZE);
    
    // Create a pooled pointer
    auto ptr = make_pooled(pool, 42);
    
    // Check that allocation succeeded
    ASSERT_NE(nullptr, ptr.get());
    EXPECT_EQ(42, ptr->getValue());
    
    // Check pool stats
    EXPECT_EQ(1, pool.getAllocatedCount());
    
    // Modify the object
    ptr->setValue(100);
    EXPECT_EQ(100, ptr->getValue());
    
    // Reset the pointer (should deallocate)
    ptr.reset();
    
    // Check that the object was deallocated
    EXPECT_EQ(nullptr, ptr.get());
    EXPECT_EQ(0, pool.getAllocatedCount());
}

// Test multiple PooledPtr
TEST_F(MemoryPoolTest, MultiplePooledPtr) {
    // Create a pool
    constexpr size_t POOL_SIZE = 10;
    ObjectPool<TestObject> pool(POOL_SIZE);
    
    // Create multiple pooled pointers
    std::vector<PooledPtr<TestObject>> pointers;
    
    // Allocate 5 objects
    for (int i = 0; i < 5; ++i) {
        pointers.push_back(make_pooled(pool, i * 10));
    }
    
    // Check pool stats
    EXPECT_EQ(5, pool.getAllocatedCount());
    EXPECT_EQ(POOL_SIZE - 5, pool.getFreeCount());
    
    // Check values
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(i * 10, pointers[i]->getValue());
    }
    
    // Release some pointers
    pointers[1].reset();
    pointers[3].reset();
    
    // Check pool stats
    EXPECT_EQ(3, pool.getAllocatedCount());
    EXPECT_EQ(POOL_SIZE - 3, pool.getFreeCount());
    
    // Allocate more objects
    pointers.push_back(make_pooled(pool, 100));
    pointers.push_back(make_pooled(pool, 200));
    
    // Check pool stats
    EXPECT_EQ(5, pool.getAllocatedCount());
    EXPECT_EQ(POOL_SIZE - 5, pool.getFreeCount());
    
    // Clear all pointers
    pointers.clear();
    
    // Check pool stats - all objects should be deallocated
    EXPECT_EQ(0, pool.getAllocatedCount());
    EXPECT_EQ(POOL_SIZE, pool.getFreeCount());
}