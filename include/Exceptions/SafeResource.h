// SafeResource.h - Exception-safe RAII wrappers
#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace GameExceptions {

/**
 * @brief RAII wrapper for any resource requiring custom cleanup
 * 
 * This template helps ensure resources are properly released even when 
 * exceptions occur. Simply provide the resource and a cleanup function.
 */
template <typename T>
class SafeResource {
public:
    // Constructor with resource and cleanup function
    SafeResource(T resource, std::function<void(T&)> cleanup)
        : m_resource(std::move(resource)), m_cleanup(std::move(cleanup)), m_active(true) {
    }
    
    // Move constructor
    SafeResource(SafeResource&& other) noexcept
        : m_resource(std::move(other.m_resource)), 
          m_cleanup(std::move(other.m_cleanup)), 
          m_active(other.m_active) {
        other.m_active = false;
    }
    
    // Move assignment
    SafeResource& operator=(SafeResource&& other) noexcept {
        if (this != &other) {
            release();
            m_resource = std::move(other.m_resource);
            m_cleanup = std::move(other.m_cleanup);
            m_active = other.m_active;
            other.m_active = false;
        }
        return *this;
    }
    
    // No copy operations
    SafeResource(const SafeResource&) = delete;
    SafeResource& operator=(const SafeResource&) = delete;
    
    // Destructor calls cleanup function
    ~SafeResource() {
        release();
    }
    
    // Access the resource
    T& get() { return m_resource; }
    const T& get() const { return m_resource; }
    
    // Release resource manually
    void release() {
        if (m_active) {
            m_cleanup(m_resource);
            m_active = false;
        }
    }
    
    // Check if resource is active
    bool isActive() const { return m_active; }
    
    // Release ownership without calling cleanup
    T release_ownership() {
        m_active = false;
        return std::move(m_resource);
    }
    
private:
    T m_resource;
    std::function<void(T&)> m_cleanup;
    bool m_active;
};

/**
 * @brief Helper for deferring code execution until scope exit
 * 
 * This is useful for ensuring cleanup happens even if exceptions are thrown.
 */
class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> onExit)
        : m_onExit(std::move(onExit)), m_active(true) {
    }
    
    // Move operations
    ScopeGuard(ScopeGuard&& other) noexcept
        : m_onExit(std::move(other.m_onExit)), m_active(other.m_active) {
        other.m_active = false;
    }
    
    ScopeGuard& operator=(ScopeGuard&& other) noexcept {
        if (this != &other) {
            dismiss();
            m_onExit = std::move(other.m_onExit);
            m_active = other.m_active;
            other.m_active = false;
        }
        return *this;
    }
    
    // No copy operations
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
    // Destructor executes the exit function
    ~ScopeGuard() {
        if (m_active && m_onExit) {
            m_onExit();
        }
    }
    
    // Cancel the exit action
    void dismiss() {
        m_active = false;
    }
    
private:
    std::function<void()> m_onExit;
    bool m_active;
};

// Macro for creating scope guards with unique names
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define SCOPE_EXIT(code) \
    GameExceptions::ScopeGuard CONCAT(scopeGuard, __LINE__) \
    ([&]() { code; })

/**
 * @brief Collection of resources that need cleanup
 * 
 * This class helps manage multiple resources that need to be cleaned up
 * in reverse order of acquisition.
 */
class ResourceManager {
public:
    // Add a cleanup function to be called on destruction
    template <typename F>
    void addCleanup(F&& cleanup) {
        m_cleanups.emplace_back(std::forward<F>(cleanup));
    }
    
    // Destructor calls all cleanup functions in reverse order
    ~ResourceManager() {
        for (auto it = m_cleanups.rbegin(); it != m_cleanups.rend(); ++it) {
            try {
                (*it)();
            } 
            catch (...) {
                // Log but continue with other cleanups
                // We don't want exceptions during cleanup to prevent other resources from being cleaned up
            }
        }
    }
    
    // Clear all cleanup functions
    void clear() {
        m_cleanups.clear();
    }
    
private:
    std::vector<std::function<void()>> m_cleanups;
};

} // namespace GameExceptions