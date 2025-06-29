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

    constexpr float bodyWidth  = TILE_SIZE * 0.3f;
    constexpr float bodyHeight = TILE_SIZE * 0.8f;

    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(bodyWidth, bodyHeight);
    physics->setPosition(
        x + TILE_SIZE / 2.f,
        y + TILE_SIZE - bodyHeight / 2.f);

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("cactus.png"));
    auto& sprite = render->getSprite();
    sf::Vector2u texSize = sprite.getTexture()->getSize();
    float scaleX = bodyWidth  / static_cast<float>(texSize.x);
    float scaleY = bodyHeight / static_cast<float>(texSize.y);
    sprite.setScale(scaleX, scaleY);
    sprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Hazard);
}
