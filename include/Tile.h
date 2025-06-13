// Tile.h
#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>




class Tile {
public:
    virtual ~Tile() = default;

    // Called every frame if needed (for animated tiles or logic)
    virtual void update(float dt) = 0;

    // Draw the tile
    virtual void render(sf::RenderTarget& target) const = 0;

    // Get the bounding box of the tile (for physics or logic)
    virtual sf::FloatRect getBounds() const = 0;

    // Flag to indicate if this tile should be collidable
    virtual bool isSolid() const = 0;
};
