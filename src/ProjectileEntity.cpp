// ProjectileEntity.cpp
#include "ProjectileEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
#include <iostream>

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

    // Kinematic bodies لا تتأثر بالجاذبية ولكن يمكنها التصادم
    auto* physics = addComponent<PhysicsComponent>(world, b2_kinematicBody);
    float projectileSize = 6.0f;
    physics->createCircleShape(projectileSize);
    physics->setPosition(x, y);

    if (auto* body = physics->getBody()) {
        b2Fixture* fixture = body->GetFixtureList();
        if (fixture) {
            b2Filter filter;
            filter.categoryBits = 0x0002; // فئة Projectile
            filter.maskBits = 0xFFFF & ~0x0004; // لا يتصادم مع فئة Enemy
            fixture->SetFilterData(filter);
        }
    }

    // Set velocity for kinematic body
    float speed = 5.0f; 
    physics->setVelocity(direction.x * speed, direction.y * speed);

    if (auto* body = physics->getBody()) {
        // Kinematic bodies ignore gravity automatically
        body->SetBullet(true);              // للتصادم السريع
        body->SetSleepingAllowed(false);    // لا تنام أبداً
        body->SetAwake(true);               // مستيقظة دائماً

        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    // Rendering
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("Bullet.png"));
    auto& sprite = render->getSprite();
    sprite.setScale(0.07f, 0.07f); 

    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Projectile);
}

void ProjectileEntity::update(float dt) {
    Entity::update(dt);

    // Update lifetime
    m_lifetime -= dt;
    if (m_lifetime <= 0) {
        setActive(false);
        return;
    }

    // Update sprite position to match physics
    auto* physics = getComponent<PhysicsComponent>();
    auto* render = getComponent<RenderComponent>();
    auto* transform = getComponent<Transform>();

    if (physics && render && transform) {
        sf::Vector2f pos = physics->getPosition();
        transform->setPosition(pos);
        render->getSprite().setPosition(pos);
    }

    // Check if projectile is off-screen and destroy it
    auto* transform2 = getComponent<Transform>();
    if (transform2) {
        sf::Vector2f pos = transform2->getPosition();

        // Destroy if way off screen
        if (pos.x < -100.0f || pos.x > WINDOW_WIDTH + 100.0f ||
            pos.y < -100.0f || pos.y > WINDOW_HEIGHT + 100.0f) {
            setActive(false);
        }
    }
}