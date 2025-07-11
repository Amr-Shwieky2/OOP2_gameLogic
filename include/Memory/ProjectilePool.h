#pragma once
#include "Memory/ObjectPool.h"
#include "Memory/PooledPtr.h"
#include "ProjectileEntity.h"
#include <Box2D/Box2D.h>
#include <SFML/System/Vector2.hpp>

/**
 * ProjectilePool - Specialized pool for efficient projectile management
 * 
 * This class manages a memory pool specifically for ProjectileEntity objects,
 * providing a convenient interface for creating projectiles with various parameters.
 */
class ProjectilePool {
public:
    // Singleton access
    static ProjectilePool& getInstance();
    
    /**
     * Initialize the projectile pool
     * 
     * @param world Reference to the Box2D world
     * @param textures Reference to the texture manager
     * @param initialCapacity Initial capacity of the pool
     */
    void initialize(b2World& world, TextureManager& textures, size_t initialCapacity = 64);
    
    /**
     * Create a projectile entity from the pool
     * 
     * @param entityId The entity ID to assign
     * @param x X position
     * @param y Y position
     * @param direction Direction vector
     * @param fromPlayer Whether the projectile is from the player
     * @param withGravity Whether the projectile is affected by gravity
     * @return A PooledPtr to the created projectile
     */
    PooledPtr<ProjectileEntity> createProjectile(
        Entity::IdType entityId,
        float x, float y,
        const sf::Vector2f& direction,
        bool fromPlayer = true,
        bool withGravity = false);
    
    /**
     * Print pool statistics
     */
    void printStats() const;
    
    /**
     * Get the number of active projectiles
     * 
     * @return Active projectile count
     */
    size_t getActiveProjectileCount() const;

private:
    // Private constructor for singleton
    ProjectilePool() = default;
    
    // The object pool for projectiles
    ObjectPool<ProjectileEntity> m_projectilePool;
    
    // References needed for projectile creation
    b2World* m_world = nullptr;
    TextureManager* m_textures = nullptr;
    
    // Statistics
    std::atomic<size_t> m_activeProjectiles{0};
    std::atomic<size_t> m_totalCreated{0};
    std::atomic<size_t> m_totalDestroyed{0};
};