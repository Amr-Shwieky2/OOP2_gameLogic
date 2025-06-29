// CactusEntity.cpp
#include "CactusEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"

CactusEntity::CactusEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id) {
    setupComponents(world, x, y, textures);
}

void CactusEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    addComponent<Transform>(sf::Vector2f(x, y));

    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(TILE_SIZE * 0.3f, TILE_SIZE * 0.8f);
    physics->setPosition(
        x + TILE_SIZE / 2.f,
        y + TILE_SIZE - (TILE_SIZE * 0.8f) / 2.f);

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("cactus.png"));
    auto& sprite = render->getSprite();
    sprite.setScale(0.1f, 0.1f);
    sprite.setOrigin(BOX_SIZE / 2.0f, BOX_SIZE / 2.0f);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Hazard);
}
