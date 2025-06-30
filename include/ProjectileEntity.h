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
        bool fromPlayer = true);

    bool isFromPlayer() const { return m_fromPlayer; }
    void update(float dt);

private:
    void setupComponents(b2World& world, float x, float y,
        sf::Vector2f direction, TextureManager& textures);
    bool m_fromPlayer;
    float m_lifetime = 3.0f; // Projectile lives for 3 seconds
};