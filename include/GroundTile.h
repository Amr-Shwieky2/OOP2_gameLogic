#pragma once

#include "Tile.h"
#include "TileType.h"
#include "ResourceManager.h"
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

static constexpr float TILE_SIZE = 128.f; // pixels

class GroundTile : public Tile {
public:
    GroundTile(b2World& world, float x, float y, TileType type, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    void update(float dt) override;
    sf::FloatRect getBounds() const override;
    bool isSolid() const override;

    void setPhysicsEnabled(bool enabled); // <-- NEW

private:
    TileType m_type;
    sf::Sprite m_sprite;
    b2Body* m_body = nullptr;
    bool m_hasPhysics = true; // <-- NEW
};
