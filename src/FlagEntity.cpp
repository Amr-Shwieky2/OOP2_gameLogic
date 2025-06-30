// FlagEntity.cpp
#include "FlagEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"

FlagEntity::FlagEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id) {
    setupComponents(world, x, y, textures);
}

void FlagEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    addComponent<Transform>(sf::Vector2f(x, y));

    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(TILE_SIZE / 2.f, TILE_SIZE);
    physics->setPosition(x + TILE_SIZE / 2.f, y + TILE_SIZE / 2.f);

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("redFlag.png"));
    render->getSprite().setPosition(x, y);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Collectible);
}

void FlagEntity::onPlayerReach() {
    // Level complete - handled in collision system
}

