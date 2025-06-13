#pragma once

#include <SFML/Graphics.hpp>
#include "Tile.h"
#include "ResourceManager.h"

class Flag : public Tile {
public:
    Flag(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    bool isSolid() const override;
    sf::FloatRect getBounds() const override;

    void update(float);

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
};
