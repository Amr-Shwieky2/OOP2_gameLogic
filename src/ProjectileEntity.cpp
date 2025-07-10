// ProjectileEntity.cpp
#include "ProjectileEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
#include <iostream>
#include "GameSession.h"

extern GameSession* g_currentSession;

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

    // Increase projectile speed for better visibility
    float speed = 10.0f; // Increased from 5.0f for better visibility
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
    
    // Make the bullet larger and rotate in the direction of movement
    sprite.setScale(0.07f, 0.07f);  // Increased from 0.07f for better visibility
    
    // Set rotation based on direction
    float angle = atan2(direction.y, direction.x) * 180.0f / 3.14159f;
    sprite.setRotation(angle);

    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Set initial position
    sprite.setPosition(x, y);

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

    // Check if projectile is off-screen relative to the camera view and destroy it
    if (!g_currentSession) return;

    auto* playerEntity = g_currentSession->getPlayer();
    if (!playerEntity) return;

    auto* playerTransform = playerEntity->getComponent<Transform>();
    if (!playerTransform) return;

    auto* projectileTransform = getComponent<Transform>();
    if (!projectileTransform) return;

    sf::Vector2f playerPos = playerTransform->getPosition();
    sf::Vector2f projectilePos = projectileTransform->getPosition();

    // Calculate camera bounds based on player position
    float cameraLeft = playerPos.x - WINDOW_WIDTH / 2.0f;
    float cameraRight = playerPos.x + WINDOW_WIDTH / 2.0f;
    float cameraTop = playerPos.y - WINDOW_HEIGHT / 2.0f;
    float cameraBottom = playerPos.y + WINDOW_HEIGHT / 2.0f;

    // Add extra margin (300 pixels in each direction)
    const float margin = 300.0f;
    
    // Destroy if way off screen relative to camera
    if (projectilePos.x < cameraLeft - margin || projectilePos.x > cameraRight + margin ||
        projectilePos.y < cameraTop - margin || projectilePos.y > cameraBottom + margin) {
        setActive(false);
    }
}