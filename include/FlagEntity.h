// FlagEntity.h
#pragma once
#include "Entity.h"
#include <Box2D/Box2D.h>
#include "ResourceManager.h"

/**
 * FlagEntity - Level end marker
 */
class FlagEntity : public Entity {
public:
    FlagEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    void onPlayerReach();

private:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures);
};