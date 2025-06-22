#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "StaticGameObject.h"
#include "TileType.h"
#include "ResourceManager.h"
#include "Constants.h"

class GroundTile : public StaticGameObject {
public:
    GroundTile(b2World& world, float x, float y, TileType type, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

private:
    std::string getTextureName(TileType type) const;

    sf::Sprite m_sprite;
    sf::FloatRect m_bounds;
    TileType m_type;
    b2Body* m_body = nullptr;
};
