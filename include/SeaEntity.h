// SeaEntity.h
#pragma once
#include "Entity.h"
#include <Box2D/Box2D.h>
#include "ResourceManager.h"

/**
 * SeaEntity - Water hazard that kills player on contact
 */
class SeaEntity : public Entity {
public:
    SeaEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    void onPlayerContact();

private:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures);
};