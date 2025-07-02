#include "FalconEnemyEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "ProjectileEntity.h"
#include "GameSession.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

extern int g_nextEntityId;
extern GameSession* g_currentSession;

FalconEnemyEntity::FalconEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : EnemyEntity(id, EnemyType::Falcon, world, x, y, textures) {
    std::cout << "[FALCON ENEMY] Constructor called" << std::endl;
    setupComponents(world, x, y, textures);
}

void FalconEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    // Call base setup
    EnemyEntity::setupComponents(world, x, y, textures);

    // Position high in the sky
    float centerX = x + TILE_SIZE / 2.f;
    float skyY = 150.0f; // Flying altitude

    // Update transform position
    auto* transform = getComponent<Transform>();
    if (transform) {
        transform->setPosition(centerX, skyY);
    }

    // Add physics - flying enemy
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(TILE_SIZE * 0.8f, TILE_SIZE * 0.6f);
    physics->setPosition(centerX, skyY);

    if (auto* body = physics->getBody()) {
        body->SetGravityScale(0.0f); // No gravity
        body->SetFixedRotation(true);
        body->SetLinearDamping(0.2f); // Slight air resistance
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    // Load wing animation textures
    try {
        m_texture1 = &textures.getResource("FalconEnemy.png");
        m_texture2 = &textures.getResource("FalconEnemy2.png");
        std::cout << "[FALCON] Loaded both wing textures" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[FALCON] Error loading textures: " << e.what() << std::endl;
        m_texture1 = m_texture2 = &textures.getResource("FalconEnemy.png");
    }

    // Setup rendering
    auto* render = addComponent<RenderComponent>();
    render->setTexture(*m_texture1);
    auto& sprite = render->getSprite();
    sprite.setScale(0.2f, 0.2f);
    sprite.setColor(sf::Color::White);

    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite.setPosition(centerX, skyY);

    // Enhanced health
    auto* health = getComponent<HealthComponent>();
    if (health) {
        health->setHealth(3);
    }

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Enemy);
    std::cout << "[FALCON] Setup complete at altitude " << skyY << std::endl;
}

void FalconEnemyEntity::update(float dt) {
    EnemyEntity::update(dt);

    updateAnimation(dt);
    updateFlightPattern(dt);
    updateShooting(dt);

    // Sync visual position
    auto* physics = getComponent<PhysicsComponent>();
    auto* render = getComponent<RenderComponent>();
    if (physics && render) {
        sf::Vector2f pos = physics->getPosition();
        render->getSprite().setPosition(pos);
    }
}

void FalconEnemyEntity::updateAnimation(float dt) {
    m_animationTimer += dt;
    if (m_animationTimer >= m_animationSpeed) {
        switchTexture();
        m_animationTimer = 0.0f;
    }
}

void FalconEnemyEntity::updateFlightPattern(float dt) {
    auto* physics = getComponent<PhysicsComponent>();
    if (!physics) return;

    sf::Vector2f currentPos = physics->getPosition();

    float horizontalSpeed = 5.0f;
    float vx = horizontalSpeed;
    float vy = 0.0f; 

    physics->setVelocity(vx, vy);

    if (currentPos.x > WINDOW_WIDTH + 200.0f) {
        physics->setPosition(-200.0f, currentPos.y);
    }
}


void FalconEnemyEntity::updateShooting(float dt) {
    m_shootTimer += dt;
    if (m_shootTimer >= m_shootCooldown) {
        shootProjectile();
        m_shootTimer = 0.0f;
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

    auto& sprite = render->getSprite();
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
}

void FalconEnemyEntity::shootProjectile() {
    if (!g_currentSession) {
        std::cerr << "[FALCON] No game session for shooting" << std::endl;
        return;
    }

    auto* transform = getComponent<Transform>();
    auto* physics = getComponent<PhysicsComponent>();

    if (!transform || !physics) {
        std::cerr << "[FALCON] Missing components for shooting" << std::endl;
        return;
    }

    b2Body* body = physics->getBody();
    if (!body) {
        std::cerr << "[FALCON] Physics body is null" << std::endl;
        return;
    }

    b2World& world = *body->GetWorld();

    sf::Vector2f falconPos = transform->getPosition();
    sf::Vector2f shootDirection(0.0f, 1.0f); 
    sf::Vector2f bulletSpawnPos = falconPos + sf::Vector2f(0.0f, 30.0f);

    try {
        // إنشاء المقذوف
        auto projectile = std::make_unique<ProjectileEntity>(
            g_nextEntityId++,
            world,
            bulletSpawnPos.x,
            bulletSpawnPos.y,
            shootDirection,
            getTextures(),  
            false          
        );

        g_currentSession->spawnEntity(std::move(projectile));
        std::cout << "[FALCON] Shot projectile downward" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "[FALCON] Error shooting: " << e.what() << std::endl;
    }
}
