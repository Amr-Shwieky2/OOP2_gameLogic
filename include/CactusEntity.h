// CactusEntity.h
#pragma once
#include "Entity.h"
#include <Box2D/Box2D.h>
#include "ResourceManager.h"

/**
 * CactusEntity - Damages player on contact
 */
class CactusEntity : public Entity {
public:
    CactusEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

private:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures);
};