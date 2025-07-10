// ProjectileEntity.h
#pragma once
#include "Entity.h"
#include <Box2D/Box2D.h>
#include "ResourceManager.h"

// ProjectileEntity.h
class ProjectileEntity : public Entity {
public:
    ProjectileEntity(IdType id, b2World& world, float x, float y,
        sf::Vector2f direction, TextureManager& textures,
        bool fromPlayer = true, bool withGravity = false);

    bool isFromPlayer() const { return m_fromPlayer; }
    void update(float dt) override;

private:
    void setupComponents(b2World& world, float x, float y,
        sf::Vector2f direction, TextureManager& textures, bool withGravity);
    bool m_fromPlayer;
    bool m_withGravity;
    float m_lifetime = 3.0f; // Projectile lives for 3 seconds
    float m_stopTime = 0.0f; // How long the projectile has been stopped
    
    // Gravity simulation parameters (for projectiles with gravity)
    sf::Vector2f m_velocity;
    static constexpr float GRAVITY = 9.8f; // Gravity constant
};