#include "Memory/ProjectilePool.h"
#include "Memory/MemoryManager.h"
#include <iostream>

ProjectilePool& ProjectilePool::getInstance() {
    static ProjectilePool instance;
    return instance;
}

void ProjectilePool::initialize(b2World& world, TextureManager& textures, size_t initialCapacity) {
    // Store references
    m_world = &world;
    m_textures = &textures;
    
    // Clear any existing projectiles
    m_activeProjectiles = 0;
    
    // Reserve space in the pool
    m_projectilePool.expand(initialCapacity);
    
    std::cout << "[ProjectilePool] Initialized with capacity " << initialCapacity << std::endl;
}

PooledPtr<ProjectileEntity> ProjectilePool::createProjectile(
    Entity::IdType entityId,
    float x, float y,
    const sf::Vector2f& direction,
    bool fromPlayer,
    bool withGravity) {
    
    // Check if references are valid
    if (!m_world || !m_textures) {
        std::cerr << "[ProjectilePool] Cannot create projectile: pool not initialized" << std::endl;
        return PooledPtr<ProjectileEntity>(nullptr, m_projectilePool);
    }
    
    // Record operation start time for profiling
    auto opId = MemoryManager::getInstance().startOperation("CreateProjectile");
    
    // Create the projectile from the pool
    ProjectileEntity* projectile = m_projectilePool.create(
        entityId, *m_world, x, y, direction, *m_textures, fromPlayer, withGravity);
    
    // Update stats
    if (projectile) {
        m_activeProjectiles++;
        m_totalCreated++;
    }
    
    // End operation profiling
    MemoryManager::getInstance().endOperation(opId);
    
    // Create a pooled pointer that will automatically return the projectile to the pool
    return PooledPtr<ProjectileEntity>(projectile, m_projectilePool);
}

void ProjectilePool::printStats() const {
    std::cout << "===== ProjectilePool Statistics =====" << std::endl;
    std::cout << "Active projectiles: " << m_activeProjectiles << std::endl;
    std::cout << "Total created: " << m_totalCreated << std::endl;
    std::cout << "Total destroyed: " << m_totalDestroyed << std::endl;
    
    // Print the pool statistics
    m_projectilePool.printStats();
}

size_t ProjectilePool::getActiveProjectileCount() const {
    return m_activeProjectiles;
}