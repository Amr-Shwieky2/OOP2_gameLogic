#include "GroundTile.h"
#include "App.h"

GroundTile::GroundTile(b2World& world, float x, float y, TileType type, TextureManager& textures)
    : m_type(type)
{
    std::string textureName;
    switch (type) {
    case TileType::Middle:     textureName = "middle.png"; break;
    case TileType::LeftEdge:   textureName = "leftEdge.png"; break;
    case TileType::RightEdge:  textureName = "rightEdge.png"; break;
    case TileType::Left:       textureName = "left.png"; break;
    case TileType::Right:      textureName = "right.png"; break;
    case TileType::Sea:        textureName = "Sea.png"; break;
    case TileType::Ground:     textureName = "ground.png"; break;
    default:                   textureName = "middle.png"; break;
    }

    sf::Texture& texture = textures.getResource(textureName);
    m_sprite.setTexture(texture);
    m_sprite.setPosition(x, y);

    // Handle tile scaling
    float scaleX = TILE_SIZE / texture.getSize().x;
    float scaleY = TILE_SIZE / texture.getSize().y;

    if (type == TileType::LeftEdge || type == TileType::RightEdge) {
        scaleY *= 2.0f; // Make tall tiles double height
    }

    m_sprite.setScale(scaleX, scaleY);

    // Create physics body if solid
    if (isSolid()) {
        float height = TILE_SIZE;
        if (type == TileType::LeftEdge || type == TileType::RightEdge) {
            height *= 2.0f; // Match visual height
        }

        b2BodyDef bodyDef;
        bodyDef.position.Set((x + TILE_SIZE / 2) / PPM, (y + height / 2) / PPM);
        bodyDef.type = b2_staticBody;
        m_body = world.CreateBody(&bodyDef);

        b2PolygonShape boxShape;
        boxShape.SetAsBox((TILE_SIZE / 2) / PPM, (height / 2) / PPM);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &boxShape;
        fixtureDef.friction = 0.8f;
        m_body->CreateFixture(&fixtureDef);
    }
}

void GroundTile::setPhysicsEnabled(bool enabled) {
    if (!enabled && m_body) {
        m_body->GetWorld()->DestroyBody(m_body);
        m_body = nullptr;
    }
    m_hasPhysics = enabled;
}

void GroundTile::render(sf::RenderTarget& target) const {
    if (m_sprite.getTexture()) {
        target.draw(m_sprite);
    }
}

void GroundTile::update(float) {
    // Static tile, no logic needed
}

sf::FloatRect GroundTile::getBounds() const {
    return m_sprite.getGlobalBounds();
}

bool GroundTile::isSolid() const {
    return m_type != TileType::Sea && m_type != TileType::Empty;
}
