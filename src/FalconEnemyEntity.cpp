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
#include "PlayerEntity.h"

extern int g_nextEntityId;
extern GameSession* g_currentSession;

FalconEnemyEntity::FalconEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : EnemyEntity(id, EnemyType::Falcon, world, x, y, textures) {
    setupComponents(world, x, y, textures);
}

void FalconEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    EnemyEntity::setupComponents(world, x, y, textures);

    float centerX = x + TILE_SIZE / 2.f;

    // Set transform position
    auto* transform = getComponent<Transform>();
    if (transform) {
        transform->setPosition(centerX, m_flightAltitude);
    }

    // Add physics - flying enemy with no gravity
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(TILE_SIZE * 0.8f, TILE_SIZE * 0.6f);
    physics->setPosition(centerX, m_flightAltitude);

    if (auto* body = physics->getBody()) {
        body->SetGravityScale(0.0f); // No gravity for flying
        body->SetFixedRotation(true);
        body->SetLinearDamping(0.2f);
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    // Load wing animation textures
    try {
        m_texture1 = &textures.getResource("FalconEnemy.png");
        m_texture2 = &textures.getResource("FalconEnemy2.png");
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
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
    sprite.setPosition(centerX, m_flightAltitude);

    // Set health
    if (auto* health = getComponent<HealthComponent>()) {
        health->setHealth(3);
    }

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Enemy);

    // Initialize shooting state - starts disabled until visible
    m_readyToShoot = false;
    m_shootTimer = 0.0f;
    m_hasEnteredScreen = false;
}

void FalconEnemyEntity::updateFlightPattern(float dt) {
    auto* physics = getComponent<PhysicsComponent>();
    if (!physics || !g_currentSession) return;

    sf::Vector2f currentPos = physics->getPosition();

    PlayerEntity* player = g_currentSession->getPlayer();
    if (!player) return;

    auto* playerTransform = player->getComponent<Transform>();
    if (!playerTransform) return;

    sf::Vector2f playerPos = playerTransform->getPosition();
    float cameraLeft = playerPos.x - WINDOW_WIDTH / 2.0f;
    float cameraRight = playerPos.x + WINDOW_WIDTH / 2.0f;

    // Move falcon to the left
    physics->setVelocity(-3.0f, 0.0f);

    // STABLE LOGIC: Wider margins to prevent flickering
    bool inShootingZone = (currentPos.x >= cameraLeft - 200.0f && currentPos.x <= cameraRight + 200.0f);
    bool farLeft = (currentPos.x < cameraLeft - 300.0f);
    bool farRight = (currentPos.x > cameraRight + 300.0f);

    // Enable shooting when entering the zone (simplified condition)
    if (inShootingZone && !m_readyToShoot) {
        m_readyToShoot = true;
        m_hasEnteredScreen = true;
        m_shootTimer = 0.0f;
    }

    // Disable shooting only when FAR outside
    if ((farLeft || farRight) && m_readyToShoot) {
        m_readyToShoot = false;
    }

    // Loop back when very far off-screen
    if (currentPos.x < cameraLeft - 400.0f) {
        float newX = cameraRight + 300.0f;
        physics->setPosition(newX, m_flightAltitude);
        m_shootTimer = 0.0f;
        m_readyToShoot = false;
        m_hasEnteredScreen = false;
    }

    // Force active always
    if (!isActive()) {
        setActive(true);
    }

    // Debug position every 3 seconds (less spam)
    static float posDebugTimer = 0.0f;
    posDebugTimer += dt;
    if (posDebugTimer >= 3.0f) {
        posDebugTimer = 0.0f;
    }
}

// Simplified shooting - less debug spam
void FalconEnemyEntity::updateShooting(float dt) {
    if (!m_readyToShoot) {
        return;
    }

    m_shootTimer += dt;
    if (m_shootTimer >= m_shootCooldown) {
        shootProjectile();
        m_shootTimer = 0.0f;
        // Remove spam - only print occasionally
        static int shotCount = 0;
        shotCount++;
    }
}

// Clean up the update method
void FalconEnemyEntity::update(float dt) {
    // Debug every 3 seconds instead of 1
    static float debugTimer = 0.0f;
    debugTimer += dt;
    if (debugTimer >= 3.0f) {
        debugTimer = 0.0f;
        auto* physics = getComponent<PhysicsComponent>();
        if (physics) {
            sf::Vector2f pos = physics->getPosition();
        }
    }

    // Force active always
    if (!isActive()) {
        setActive(true);
    }

    EnemyEntity::update(dt);
    updateAnimation(dt);
    updateFlightPattern(dt);
    updateShooting(dt);

    // Sync visual position
    auto* physics = getComponent<PhysicsComponent>();
    auto* render = getComponent<RenderComponent>();
    if (physics && render) {
        render->getSprite().setPosition(physics->getPosition());
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
    render->setTexture(m_currentFrame == 0 ? *m_texture1 : *m_texture2);

    auto& sprite = render->getSprite();
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
}

void FalconEnemyEntity::shootProjectile() {
    if (!g_currentSession) return;

    auto* transform = getComponent<Transform>();
    auto* physics = getComponent<PhysicsComponent>();
    if (!transform || !physics) return;

    b2Body* body = physics->getBody();
    if (!body) return;

    b2World& world = *body->GetWorld();
    sf::Vector2f falconPos = transform->getPosition();
    sf::Vector2f bulletSpawnPos = falconPos + sf::Vector2f(0.0f, 60.0f);
    sf::Vector2f shootDir(0.0f, 1.0f); // Shoot downward

    try {
        // Create enemy projectile
        auto projectile = std::make_unique<ProjectileEntity>(
            g_nextEntityId++, world,
            bulletSpawnPos.x, bulletSpawnPos.y,
            shootDir, getTextures(), false // false = enemy projectile
        );
        g_currentSession->spawnEntity(std::move(projectile));
    }
    catch (const std::exception& e) {
        std::cerr << "[FALCON] Error shooting: " << e.what() << std::endl;
    }
}