#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <utility>
#include <string>
#include <vector>
#include <iostream>

/**
 * MultiMethodDispatcher - Implements multimethods for generic object interactions
 */
class MultiMethodDispatcher {
public:
    // Get the singleton instance
    static MultiMethodDispatcher& getInstance() {
        static MultiMethodDispatcher instance;
        return instance;
    }

    // Register a handler for specific object types in a domain
    template<typename T1, typename T2>
    void registerHandler(const std::string& domain, std::function<void(T1&, T2&)> handler) {
        // Create key from type indices and domain
        auto key = std::make_tuple(
            std::type_index(typeid(T1)), 
            std::type_index(typeid(T2)), 
            domain
        );

        // Store the handler
        m_handlers[key] = [handler](void* obj1, void* obj2) {
            T1& typedObj1 = *static_cast<T1*>(obj1);
            T2& typedObj2 = *static_cast<T2*>(obj2);
            handler(typedObj1, typedObj2);
        };

        std::cout << "Registered handler for " 
                  << typeid(T1).name() << " and " << typeid(T2).name() 
                  << " in domain '" << domain << "'" << std::endl;
    }

    // Process interaction between two objects in a domain
    template<typename T1, typename T2>
    bool dispatch(T1& obj1, T2& obj2, const std::string& domain) {
        // Try direct key
        auto key = std::make_tuple(
            std::type_index(typeid(obj1)), 
            std::type_index(typeid(obj2)), 
            domain
        );
        
        auto it = m_handlers.find(key);
        if (it != m_handlers.end()) {
            it->second(&obj1, &obj2);
            return true;
        }

        // Try reversed key
        auto reverseKey = std::make_tuple(
            std::type_index(typeid(obj2)), 
            std::type_index(typeid(obj1)), 
            domain
        );
        
        it = m_handlers.find(reverseKey);
        if (it != m_handlers.end()) {
            it->second(&obj2, &obj1);
            return true;
        }

        return false;
    }

private:
    // Key type: (type1, type2, domain)
    using DispatchKey = std::tuple<std::type_index, std::type_index, std::string>;
    
    // Custom hash function for DispatchKey
    struct KeyHash {
        size_t operator()(const DispatchKey& key) const {
            auto h1 = std::hash<std::type_index>{}(std::get<0>(key));
            auto h2 = std::hash<std::type_index>{}(std::get<1>(key));
            auto h3 = std::hash<std::string>{}(std::get<2>(key));
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
    
    // Custom equality function for DispatchKey
    struct KeyEqual {
        bool operator()(const DispatchKey& lhs, const DispatchKey& rhs) const {
            return std::get<0>(lhs) == std::get<0>(rhs) &&
                   std::get<1>(lhs) == std::get<1>(rhs) &&
                   std::get<2>(lhs) == std::get<2>(rhs);
        }
    };
    
    // Handler function type
    using HandlerFunction = std::function<void(void*, void*)>;
    
    // Handler map
    std::unordered_map<DispatchKey, HandlerFunction, KeyHash, KeyEqual> m_handlers;
    
    // Private constructor for singleton
    MultiMethodDispatcher() = default;
    
    // Delete copy/move constructors and assignments
    MultiMethodDispatcher(const MultiMethodDispatcher&) = delete;
    MultiMethodDispatcher& operator=(const MultiMethodDispatcher&) = delete;
    MultiMethodDispatcher(MultiMethodDispatcher&&) = delete;
    MultiMethodDispatcher& operator=(MultiMethodDispatcher&&) = delete;
};