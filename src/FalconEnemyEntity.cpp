// FalconEnemyEntity.cpp
#include "FalconEnemyEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "AIComponent.h"
#include "ProjectileEntity.h"
#include "GameSession.h"
#include "Constants.h"
#include <iostream>

extern int g_nextEntityId;

FalconEnemyEntity::FalconEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : EnemyEntity(id, EnemyType::Falcon, world, x, y, textures) {
}

void FalconEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    EnemyEntity::setupComponents(world, x, y, textures);

    // Start off-screen above the level
    float startY = -200.0f; // Above screen

    // Add transform at spawn position
    auto* transform = getComponent<Transform>();
    if (transform) {
        transform->setPosition(x + TILE_SIZE / 2.f, startY);
    }

    // Add physics - flying enemy with no gravity
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(TILE_SIZE * 1.2f, TILE_SIZE * 0.8f); // Wider for wings
    physics->setPosition(x + TILE_SIZE / 2.f, startY);

    if (auto* body = physics->getBody()) {
        body->SetGravityScale(0.0f); // No gravity - flying enemy
        body->SetFixedRotation(true);
    }

    // Add rendering with animation
    auto* render = addComponent<RenderComponent>();

    // Load both textures
    m_texture1 = &textures.getResource("FalconEnemy1.png");
    m_texture2 = &textures.getResource("FalconEnemy2.png");

    render->setTexture(*m_texture1);
    auto& sprite = render->getSprite();
    sprite.setScale(0.4f, 0.4f); // Larger than ground enemies
    sprite.setColor(sf::Color(255, 255, 255, 0)); // Start invisible

    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    // Add stronger health
    auto* health = getComponent<HealthComponent>();
    if (health) {
        health->setHealth(5); // 5 hits to kill
    }

    // No AI component initially - we'll control movement manually

    // Add collision
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Enemy);
}

void FalconEnemyEntity::update(float dt) {
    EnemyEntity::update(dt);

    // Handle spawn delay
    if (!m_spawned) {
        m_spawnTimer += dt;

        if (m_spawnTimer >= SPAWN_DELAY) {
            m_spawned = true;

            // Make visible and move to play area
            auto* render = getComponent<RenderComponent>();
            if (render) {
                render->getSprite().setColor(sf::Color::White);
            }

            // Move down into view
            auto* physics = getComponent<PhysicsComponent>();
            if (physics) {
                sf::Vector2f currentPos = physics->getPosition();
                physics->setPosition(currentPos.x, 200.0f); // Move to sky position
            }

            std::cout << "[FalconEnemy] Spawned after 30 seconds!" << std::endl;
        }
        return; // Don't update anything else until spawned
    }

    // Update animation
    updateAnimation(dt);

    // Flying pattern - move in sine wave
    auto* physics = getComponent<PhysicsComponent>();
    if (physics) {
        static float waveTimer = 0.0f;
        waveTimer += dt;

        // Horizontal movement with sine wave vertical movement
        float vx = 50.0f; // Slow horizontal movement
        float vy = std::sin(waveTimer * 2.0f) * 30.0f; // Sine wave

        physics->setVelocity(vx, vy);
    }

    // Handle shooting
    m_shootTimer += dt;
    if (m_shootTimer >= m_shootCooldown) {
        shootProjectile();
        m_shootTimer = 0.0f;
    }
}

void FalconEnemyEntity::updateAnimation(float dt) {
    m_animationTimer += dt;

    if (m_animationTimer >= m_animationSpeed) {
        switchTexture();
        m_animationTimer = 0.0f;
    }
}

void FalconEnemyEntity::switchTexture() {
    auto* render = getComponent<RenderComponent>();
    if (!render || !m_texture1 || !m_texture2) return;

    m_currentFrame = (m_currentFrame + 1) % 2;

    if (m_currentFrame == 0) {
        render->setTexture(*m_texture1);
    }
    else {
        render->setTexture(*m_texture2);
    }

    // Re-center origin after texture switch
    auto& sprite = render->getSprite();
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
}

void FalconEnemyEntity::shootProjectile() {
    auto* transform = getComponent<Transform>();
    if (!transform) return;

    sf::Vector2f pos = transform->getPosition();

    // Shoot downward
    sf::Vector2f shootDirection(0.0f, 1.0f);

    // Create projectile
    // Note: This needs access to the game session to spawn the projectile
    // For now, just log it
    std::cout << "[FalconEnemy] Shooting projectile from ("
        << pos.x << ", " << pos.y << ")" << std::endl;

    // TODO: Access EntityManager to spawn projectile
    // EntityManager would need to be accessible, perhaps through a singleton
    // or by storing a reference in the enemy
}