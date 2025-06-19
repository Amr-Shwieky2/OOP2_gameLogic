#include "GroundTile.h"
#include "GameObjectVisitor.h"

GroundTile::GroundTile(b2World& world, float x, float y, TileType type, TextureManager& textures)
    : m_type(type)
{
    sf::Texture& tex = textures.getResource(getTextureName(type));
    m_sprite.setTexture(tex);
    m_sprite.setPosition(x, y);

    // Store the bounds for collision
    m_bounds = m_sprite.getGlobalBounds();

    // Create static Box2D body
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set((x + m_bounds.width / 2.f) / PPM, (y + m_bounds.height / 2.f) / PPM);
    m_body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox((m_bounds.width / 2.f) / PPM, (m_bounds.height / 2.f) / PPM);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.friction = 0.6f;
    m_body->CreateFixture(&fixtureDef);
}

void GroundTile::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect GroundTile::getBounds() const {
    return m_bounds;
}

void GroundTile::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

std::string GroundTile::getTextureName(TileType type) const {
    switch (type) {
    case TileType::Ground:      return "ground.png";
    case TileType::Middle:      return "middle.png";
    case TileType::Left:        return "left.png";
    case TileType::Right:       return "right.png";
    case TileType::LeftEdge:    return "leftEdge.png";
    case TileType::RightEdge:   return "rightEdge.png";
    case TileType::Sea:         return "sea.png";
    default:                    return "missing.png";
    }
}
