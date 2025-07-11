#pragma once
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cassert>
#include <type_traits>
#include <algorithm>

/**
 * ComponentArray - A cache-friendly dense array for storing components
 * 
 * This class stores components of a specific type in a contiguous memory layout,
 * optimizing for cache locality and iteration performance. It uses a mapping from
 * entity IDs to array indices for fast lookup.
 * 
 * @tparam T The component type to store
 * @tparam IdType The type used for entity IDs
 */
template<typename T, typename IdType = uint32_t>
class ComponentArray {
public:
    // Ensure T is not a reference or const/volatile qualified
    static_assert(!std::is_reference<T>::value && !std::is_const<T>::value && !std::is_volatile<T>::value,
                 "T cannot be reference, const, or volatile");
    
    /**
     * Default constructor
     */
    ComponentArray() = default;
    
    /**
     * Constructor with initial capacity
     * 
     * @param initialCapacity The initial capacity of the array
     */
    explicit ComponentArray(size_t initialCapacity) {
        m_components.reserve(initialCapacity);
    }
    
    /**
     * Get the number of components in the array
     * 
     * @return Component count
     */
    size_t size() const {
        return m_components.size();
    }
    
    /**
     * Check if the array is empty
     * 
     * @return true if the array is empty
     */
    bool empty() const {
        return m_components.empty();
    }
    
    /**
     * Insert a component for an entity
     * 
     * @param entityId The entity ID
     * @param component The component to insert
     * @return Reference to the inserted component
     */
    T& insert(IdType entityId, const T& component) {
        // Check if the entity already has this component
        auto it = m_entityToIndex.find(entityId);
        if (it != m_entityToIndex.end()) {
            // Update existing component
            m_components[it->second] = component;
            return m_components[it->second];
        }
        
        // Add new component
        size_t newIndex = m_components.size();
        m_entityToIndex[entityId] = newIndex;
        m_indexToEntity[newIndex] = entityId;
        m_components.push_back(component);
        
        return m_components.back();
    }
    
    /**
     * Insert a component for an entity (move version)
     * 
     * @param entityId The entity ID
     * @param component The component to insert (will be moved)
     * @return Reference to the inserted component
     */
    T& insert(IdType entityId, T&& component) {
        // Check if the entity already has this component
        auto it = m_entityToIndex.find(entityId);
        if (it != m_entityToIndex.end()) {
            // Update existing component
            m_components[it->second] = std::move(component);
            return m_components[it->second];
        }
        
        // Add new component
        size_t newIndex = m_components.size();
        m_entityToIndex[entityId] = newIndex;
        m_indexToEntity[newIndex] = entityId;
        m_components.push_back(std::move(component));
        
        return m_components.back();
    }
    
    /**
     * Emplace a component for an entity by constructing it in-place
     * 
     * @param entityId The entity ID
     * @param args Constructor arguments for the component
     * @return Reference to the emplaced component
     */
    template<typename... Args>
    T& emplace(IdType entityId, Args&&... args) {
        // Check if the entity already has this component
        auto it = m_entityToIndex.find(entityId);
        if (it != m_entityToIndex.end()) {
            // Replace existing component
            m_components[it->second] = T(std::forward<Args>(args)...);
            return m_components[it->second];
        }
        
        // Add new component
        size_t newIndex = m_components.size();
        m_entityToIndex[entityId] = newIndex;
        m_indexToEntity[newIndex] = entityId;
        m_components.emplace_back(std::forward<Args>(args)...);
        
        return m_components.back();
    }
    
    /**
     * Remove a component for an entity
     * 
     * @param entityId The entity ID
     * @return true if a component was removed
     */
    bool remove(IdType entityId) {
        auto it = m_entityToIndex.find(entityId);
        if (it == m_entityToIndex.end()) {
            return false; // Entity doesn't have this component
        }
        
        // Get the index of the component to remove
        size_t indexToRemove = it->second;
        
        // If it's not the last element, move the last element to this position
        // to maintain a packed array
        size_t lastIndex = m_components.size() - 1;
        if (indexToRemove != lastIndex) {
            // Move the last component to the position of the removed component
            m_components[indexToRemove] = std::move(m_components[lastIndex]);
            
            // Update the mappings for the moved component
            IdType movedEntityId = m_indexToEntity[lastIndex];
            m_entityToIndex[movedEntityId] = indexToRemove;
            m_indexToEntity[indexToRemove] = movedEntityId;
        }
        
        // Remove the last element (which is now either the removed component or a duplicate)
        m_components.pop_back();
        
        // Remove the mappings for the removed entity
        m_indexToEntity.erase(lastIndex);
        m_entityToIndex.erase(entityId);
        
        return true;
    }
    
    /**
     * Get a component for an entity
     * 
     * @param entityId The entity ID
     * @return Pointer to the component, or nullptr if not found
     */
    T* get(IdType entityId) {
        auto it = m_entityToIndex.find(entityId);
        if (it == m_entityToIndex.end()) {
            return nullptr;
        }
        
        return &m_components[it->second];
    }
    
    /**
     * Get a component for an entity (const version)
     * 
     * @param entityId The entity ID
     * @return Pointer to the component, or nullptr if not found
     */
    const T* get(IdType entityId) const {
        auto it = m_entityToIndex.find(entityId);
        if (it == m_entityToIndex.end()) {
            return nullptr;
        }
        
        return &m_components[it->second];
    }
    
    /**
     * Check if an entity has a component
     * 
     * @param entityId The entity ID
     * @return true if the entity has this component
     */
    bool has(IdType entityId) const {
        return m_entityToIndex.find(entityId) != m_entityToIndex.end();
    }
    
    /**
     * Apply a function to each component
     * 
     * @param func Function to apply (gets component reference)
     */
    template<typename Func>
    void forEach(Func&& func) {
        for (auto& component : m_components) {
            func(component);
        }
    }
    
    /**
     * Apply a function to each entity-component pair
     * 
     * @param func Function to apply (gets entity ID and component reference)
     */
    template<typename Func>
    void forEachWithEntity(Func&& func) {
        for (size_t i = 0; i < m_components.size(); ++i) {
            func(m_indexToEntity[i], m_components[i]);
        }
    }
    
    /**
     * Clear all components
     */
    void clear() {
        m_components.clear();
        m_entityToIndex.clear();
        m_indexToEntity.clear();
    }
    
    /**
     * Direct access to the component array for efficient iteration
     * WARNING: Do not modify the array structure through this reference
     * 
     * @return Reference to the internal component array
     */
    const std::vector<T>& getRawData() const {
        return m_components;
    }
    
    /**
     * Iterator access for range-based for loops
     */
    auto begin() { return m_components.begin(); }
    auto end() { return m_components.end(); }
    auto begin() const { return m_components.begin(); }
    auto end() const { return m_components.end(); }

private:
    // The actual components stored in a contiguous array
    std::vector<T> m_components;
    
    // Maps entity IDs to component indices
    std::unordered_map<IdType, size_t> m_entityToIndex;
    
    // Maps component indices to entity IDs
    std::unordered_map<size_t, IdType> m_indexToEntity;
};