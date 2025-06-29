// BoxEntity.h
#pragma once
#include "Entity.h"
#include <Box2D/Box2D.h>
#include "ResourceManager.h"

/**
 * BoxEntity - Movable box that player can push
 */
class BoxEntity : public Entity {
public:
    BoxEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

private:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures);
};
