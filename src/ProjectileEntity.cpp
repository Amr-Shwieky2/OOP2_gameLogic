// src/ProjectileEntity.cpp
#include "ProjectileEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"

ProjectileEntity::ProjectileEntity(IdType id, b2World& world, float x, float y,
    sf::Vector2f direction, TextureManager& textures,
    bool fromPlayer)
    : Entity(id)
    , m_fromPlayer(fromPlayer) {
    setupComponents(world, x, y, direction, textures);
}

void ProjectileEntity::setupComponents(b2World& world, float x, float y,
    sf::Vector2f direction, TextureManager& textures) {
    // Add transform
    addComponent<Transform>(sf::Vector2f(x, y));

    // Add physics
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createCircleShape(10.0f); // Small projectile
    physics->setPosition(x, y);

    // Set velocity
    float speed = 500.0f; // Fast projectile
    physics->setVelocity(direction.x * speed, direction.y * speed);
    physics->getBody()->SetGravityScale(0); // No gravity for projectiles

    // Add rendering
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource(m_fromPlayer ? "Bullet.png" : "Bullet.png"));
    auto& sprite = render->getSprite();
    sprite.setScale(0.1f, 0.1f);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    // Add collision
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Projectile);
}

void ProjectileEntity::update(float dt) {
    Entity::update(dt);

    // Destroy projectile after lifetime expires
    m_lifetime -= dt;
    if (m_lifetime <= 0) {
        setActive(false);
    }
}