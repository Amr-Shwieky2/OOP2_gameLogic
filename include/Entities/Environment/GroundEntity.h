// GroundEntity.h
#pragma once
#include "Entity.h"
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "TileType.h"
#include "ResourceManager.h"

/**
 * GroundEntity - Represents ground tiles as entities
 * Replaces GroundTile static objects
 */
class GroundEntity : public Entity {
public:
    GroundEntity(IdType id, TileType type, b2World& world, float x, float y, TextureManager& textures);

    TileType getTileType() const { return m_tileType; }

private:
    void setupComponents(TileType type, b2World& world, float x, float y, TextureManager& textures);
    std::string getTextureNameForType(TileType type) const;

    TileType m_tileType;
};