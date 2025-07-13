#include "GroundEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"


GroundEntity::GroundEntity(IdType id, TileType type, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id)
    , m_tileType(type) {
    setupComponents(type, world, x, y, textures);
}

void GroundEntity::setupComponents(TileType type, b2World& world, float x, float y, TextureManager& textures) {
    std::string textureName = getTextureNameForType(type);
    sf::Texture& texture = textures.getResource(textureName);
    sf::Vector2u texSize = texture.getSize();
    float texWidth = static_cast<float>(texSize.x);
    float texHeight = static_cast<float>(texSize.y);

    float boxWidth = TILE_SIZE;
    float boxHeight = TILE_SIZE;
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f;

    if (type == TileType::Edge) {
        boxWidth = texWidth;
        boxHeight = texHeight;

        centerX = x + boxWidth / 2.f;
        centerY = y + TILE_SIZE - (boxHeight / 2.f);
    }

    addComponent<Transform>(sf::Vector2f(centerX, centerY));

    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(boxWidth, boxHeight);
    physics->setPosition(centerX, centerY);

    auto* render = addComponent<RenderComponent>();
    render->setTexture(texture);
    auto& sprite = render->getSprite();
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(centerX, centerY);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Ground);
}

std::string GroundEntity::getTextureNameForType(TileType type) const {
    switch (type) {
    case TileType::Ground:  return "ground.png";
    case TileType::Middle:  return "middle.png";
    case TileType::Left:    return "left.png";
    case TileType::Right:   return "right.png";
    case TileType::Edge:    return "Edge.png";
    default:                return "ground.png";
    }
}
