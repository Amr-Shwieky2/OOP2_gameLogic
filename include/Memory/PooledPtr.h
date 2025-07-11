#pragma once
#include "ObjectPool.h"
#include <functional>
#include <utility>

/**
 * PooledPtr - A smart pointer for objects allocated from an ObjectPool
 * 
 * This class provides RAII semantics for objects allocated from a memory pool,
 * automatically returning the object to the pool when the pointer goes out of scope.
 * 
 * @tparam T The type of object being pointed to
 */
template<typename T>
class PooledPtr {
public:
    /**
     * Constructor - takes ownership of an object
     * 
     * @param ptr Pointer to the object
     * @param pool Reference to the pool that allocated the object
     */
    PooledPtr(T* ptr, ObjectPool<T>& pool) : m_ptr(ptr), m_pool(&pool) {}
    
    /**
     * Move constructor - transfers ownership from another PooledPtr
     */
    PooledPtr(PooledPtr&& other) noexcept 
        : m_ptr(other.m_ptr), m_pool(other.m_pool) {
        other.m_ptr = nullptr;
        other.m_pool = nullptr;
    }
    
    /**
     * Move assignment operator - transfers ownership from another PooledPtr
     */
    PooledPtr& operator=(PooledPtr&& other) noexcept {
        if (this != &other) {
            reset();
            m_ptr = other.m_ptr;
            m_pool = other.m_pool;
            other.m_ptr = nullptr;
            other.m_pool = nullptr;
        }
        return *this;
    }
    
    /**
     * Destructor - returns the object to the pool
     */
    ~PooledPtr() {
        reset();
    }
    
    // Deleted copy constructor and assignment
    PooledPtr(const PooledPtr&) = delete;
    PooledPtr& operator=(const PooledPtr&) = delete;
    
    /**
     * Release ownership of the object without destroying it
     * 
     * @return The pointer that was owned by this PooledPtr
     */
    T* release() {
        T* ptr = m_ptr;
        m_ptr = nullptr;
        return ptr;
    }
    
    /**
     * Reset the pointer, returning the current object to the pool if any
     */
    void reset() {
        if (m_ptr && m_pool) {
            m_pool->destroy(m_ptr);
        }
        m_ptr = nullptr;
    }
    
    /**
     * Dereference operator
     * 
     * @return Reference to the managed object
     */
    T& operator*() const {
        return *m_ptr;
    }
    
    /**
     * Arrow operator for accessing object members
     * 
     * @return Pointer to the managed object
     */
    T* operator->() const {
        return m_ptr;
    }
    
    /**
     * Get the raw pointer
     * 
     * @return Pointer to the managed object
     */
    T* get() const {
        return m_ptr;
    }
    
    /**
     * Check if the pointer is valid
     * 
     * @return true if the pointer is non-null
     */
    explicit operator bool() const {
        return m_ptr != nullptr;
    }

private:
    T* m_ptr = nullptr;
    ObjectPool<T>* m_pool = nullptr;
};

/**
 * Helper function to create a PooledPtr from an ObjectPool
 * 
 * @tparam T The type of object to create
 * @tparam Args Types of the constructor arguments
 * @param pool The object pool
 * @param args Constructor arguments
 * @return A PooledPtr that owns the newly created object
 */
template<typename T, typename... Args>
PooledPtr<T> make_pooled(ObjectPool<T>& pool, Args&&... args) {
    T* ptr = pool.create(std::forward<Args>(args)...);
    return PooledPtr<T>(ptr, pool);
}