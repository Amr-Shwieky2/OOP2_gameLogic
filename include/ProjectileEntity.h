// ProjectileEntity.h
#pragma once
#include "Entity.h"
#include <Box2D/Box2D.h>
#include "ResourceManager.h"
#include "Constants.h"

class ProjectileEntity : public Entity {
public:
    ProjectileEntity(IdType id, b2World& world, float x, float y,
        float dirX, float dirY, TextureManager& textures);

    void update(float dt) override;

private:
    void setupComponents(b2World& world, float x, float y,
        float dirX, float dirY, TextureManager& textures);

    float m_lifetime = PROJECTILE_LIFETIME;
};