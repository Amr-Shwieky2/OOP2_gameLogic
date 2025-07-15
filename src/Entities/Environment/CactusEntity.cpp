#include "CactusEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
//-------------------------------------------------------------------------------------
CactusEntity::CactusEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id) {
    setupComponents(world, x, y, textures);
}
//-------------------------------------------------------------------------------------
void CactusEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    constexpr float bodyWidth = TILE_SIZE * 0.4f;  
    constexpr float bodyHeight = TILE_SIZE * 0.6f;  

    // Calculate physics position
    float physicsX = x + TILE_SIZE / 2.f;
    float physicsY = y + (TILE_SIZE - bodyHeight) / 2.f + 90.0f;

    addComponent<Transform>(sf::Vector2f(physicsX, physicsY));
    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(bodyWidth, bodyHeight);
    physics->setPosition(physicsX, physicsY);

    if (auto* body = physics->getBody()) {
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("cactus.png"));
    auto& sprite = render->getSprite();

    float visualWidth = TILE_SIZE * 0.6f;   
    float visualHeight = TILE_SIZE * 0.8f; 

    sf::Vector2u texSize = sprite.getTexture()->getSize();
    float scaleX = visualWidth / static_cast<float>(texSize.x);
    float scaleY = visualHeight / static_cast<float>(texSize.y);
    sprite.setScale(scaleX, scaleY);
    sprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
    sprite.setPosition(physicsX, physicsY);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Hazard);
}
//-------------------------------------------------------------------------------------