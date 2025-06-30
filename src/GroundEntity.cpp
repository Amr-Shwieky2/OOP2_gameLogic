// GroundEntity.cpp
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
    float heightOffset = (type == TileType::Edge) ? EDGE_HEIGHT_OFFSET : 0.f;
    // Use the tile centre as the logical position so physics and rendering
    // remain in sync when the PhysicsComponent updates the Transform.
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f - heightOffset;

    // Add transform at the centre of the tile
    addComponent<Transform>(sf::Vector2f(centerX, centerY));

    // Add physics (static body for ground)
    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(TILE_SIZE, TILE_SIZE);
    physics->setPosition(centerX, centerY);

    // Add rendering
    auto* render = addComponent<RenderComponent>();
    std::string textureName = getTextureNameForType(type);
    render->setTexture(textures.getResource(textureName));
    auto& sprite = render->getSprite();
    // Centre the sprite so that it matches the transform location
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(centerX, centerY);

    // Add collision
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
