// SeaEntity.cpp
#include "SeaEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"

SeaEntity::SeaEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id) {
    setupComponents(world, x, y, textures);
}

void SeaEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    // Position is stored at the centre so physics and rendering stay aligned
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f;

    addComponent<Transform>(sf::Vector2f(centerX, centerY));

    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(TILE_SIZE, TILE_SIZE);
    physics->setPosition(centerX, centerY);

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("Sea.png"));
    auto& sprite = render->getSprite();
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(centerX, centerY);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Hazard);
}

void SeaEntity::onPlayerContact() {
    // Player dies instantly - handled in collision system
}