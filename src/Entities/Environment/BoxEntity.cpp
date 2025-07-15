#include "BoxEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
//-------------------------------------------------------------------------------------
BoxEntity::BoxEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id) {
    setupComponents(world, x, y, textures);
}
//-------------------------------------------------------------------------------------
void BoxEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    // Position centered in tile
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f;

    addComponent<Transform>(sf::Vector2f(centerX, centerY));

    // Dynamic body so it can be pushed
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(BOX_SIZE, BOX_SIZE,
        BOX_DENSITY,
        BOX_FRICTION,
        BOX_RESTITUTION);
    physics->setPosition(centerX, centerY);

    // Set physics properties
    if (auto* body = physics->getBody()) {
        body->SetFixedRotation(true); // Prevent rotation
        body->SetLinearDamping(0.5f); // Add more resistance for better control

        // Store entity pointer in body user data for collision callbacks
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("wooden_box.png"));
    auto& sprite = render->getSprite();
    sf::Vector2u texSize = sprite.getTexture()->getSize();
    float scaleX = BOX_SIZE / static_cast<float>(texSize.x);
    float scaleY = BOX_SIZE / static_cast<float>(texSize.y);
    sprite.setScale(scaleX, scaleY);
    sprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Obstacle);
}
//-------------------------------------------------------------------------------------
void BoxEntity::update(float dt) {
    Entity::update(dt);

    // Ensure sprite follows physics body
    auto* physics = getComponent<PhysicsComponent>();
    auto* render = getComponent<RenderComponent>();
    auto* transform = getComponent<Transform>();

    if (physics && render && transform) {
        sf::Vector2f pos = physics->getPosition();
        transform->setPosition(pos);
        render->getSprite().setPosition(pos);
    }
}
//-------------------------------------------------------------------------------------