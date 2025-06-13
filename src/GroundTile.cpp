// GroundTile.cpp
#include "GroundTile.h"

GroundTile::GroundTile(b2World& world, float x, float y, TileType type, TextureManager& textures)
    : m_type(type)
{
    std::string textureName;
    switch (type) {
	case TileType::Ground:
		textureName = "ground.png"; break;
    case TileType::Middle:
        textureName = "middle.png"; break;
    case TileType::LeftEdge:
        textureName = "leftEdge.png"; break;
    case TileType::RightEdge:
        textureName = "rightEdge.png"; break;
    default:
        textureName = "middle.PNG"; break;
    }

    sf::Texture& texture = textures.getResource(textureName);
    m_sprite.setTexture(texture);
    m_sprite.setPosition(x, y);

    // Box2D static body
    b2BodyDef bodyDef;
    bodyDef.position.Set((x + TILE_SIZE / 2) / 100.f, (y + TILE_SIZE / 2) / 100.f);
    bodyDef.type = b2_staticBody;
    m_body = world.CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(TILE_SIZE / 2 / 100.f, TILE_SIZE / 2 / 100.f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.friction = 0.8f;
    m_body->CreateFixture(&fixtureDef);
}

void GroundTile::update(float) {
    // No dynamic logic needed for ground
}

void GroundTile::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect GroundTile::getBounds() const {
    return m_sprite.getGlobalBounds();
}

bool GroundTile::isSolid() const {
    return true;
}
