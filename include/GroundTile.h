// GroundTile.h
#pragma once

#include "Tile.h"
#include "TileType.h"
#include "ResourceManager.h"
static constexpr float TILE_SIZE = 64.f; // pixels

class GroundTile : public Tile {
public:
    GroundTile(
        b2World& world,
        float x,
        float y,
        TileType type,
        TextureManager& textures
    );

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    bool isSolid() const override;

private:
    sf::Sprite m_sprite;
    b2Body* m_body = nullptr;
    TileType m_type;

};
