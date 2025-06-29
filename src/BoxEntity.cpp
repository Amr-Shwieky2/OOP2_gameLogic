// BoxEntity.cpp
#include "BoxEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"

BoxEntity::BoxEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id) {
    setupComponents(world, x, y, textures);
}

void BoxEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    addComponent<Transform>(sf::Vector2f(x, y));

    // Dynamic body so it can be pushed
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(BOX_SIZE, BOX_SIZE);
    physics->setPosition(x + BOX_SIZE / 2.f, y - BOX_SIZE / 2.f);

    // Set physics properties
    if (auto* body = physics->getBody()) {
        body->SetFixedRotation(true); // Prevent rotation
        body->SetLinearDamping(0.2f); // Add some resistance
    }

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("wooden_box.png"));
    auto& sprite = render->getSprite();
    sprite.setOrigin(BOX_SIZE / 2.0f, BOX_SIZE / 2.0f);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Obstacle);
}