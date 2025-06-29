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
    addComponent<Transform>(sf::Vector2f(x, y));

    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(TILE_SIZE, TILE_SIZE);
    physics->setPosition(x + TILE_SIZE / 2.f, y + TILE_SIZE / 2.f);

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("Sea.png"));
    render->getSprite().setPosition(x, y);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Hazard);
}

void SeaEntity::onPlayerContact() {
    // Player dies instantly - handled in collision system
}