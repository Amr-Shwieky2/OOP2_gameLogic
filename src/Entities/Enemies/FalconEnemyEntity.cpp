#include "FalconEnemyEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "ProjectileEntity.h"
#include "FalconWeaponSystem.h"
#include "GameSession.h"
#include "Constants.h"
#include <iostream>
#include <cmath>
#include "PlayerEntity.h"

extern GameSession* g_currentSession;
//-------------------------------------------------------------------------------------
FalconEnemyEntity::FalconEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : EnemyEntity(id, EnemyType::Falcon, world, x, y, textures) {
    setupComponents(world, x, y, textures);
    m_weaponSystem = std::make_unique<FalconWeaponSystem>(*this);
    m_weaponSystem->reset();
}
//-------------------------------------------------------------------------------------
void FalconEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    EnemyEntity::setupComponents(world, x, y, textures);

    float centerX = x + TILE_SIZE / 2.f;

    auto* transform = getComponent<Transform>();
    if (transform) {
        transform->setPosition(centerX, m_flightAltitude);
    }

    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(TILE_SIZE * 0.8f, TILE_SIZE * 0.6f);
    physics->setPosition(centerX, m_flightAltitude);

    if (auto* body = physics->getBody()) {
        body->SetGravityScale(0.0f); // No gravity for flying
        body->SetFixedRotation(true);
        body->SetLinearDamping(0.2f);
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

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
    sprite.setScale(-0.2f, 0.2f);
    sprite.setColor(sf::Color::White);
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
    sprite.setPosition(centerX, m_flightAltitude);

    if (auto* health = getComponent<HealthComponent>()) {
        health->setHealth(3);
    }
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Enemy);

    if (m_weaponSystem)
        m_weaponSystem->reset();
}
//-------------------------------------------------------------------------------------
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

    physics->setVelocity(-3.0f, 0.0f);

    bool inShootingZone = (currentPos.x >= cameraLeft - 200.0f && currentPos.x <= cameraRight + 200.0f);
    bool farLeft = (currentPos.x < cameraLeft - 400.0f);
    bool farRight = (currentPos.x > cameraRight + 400.0f);

    if (inShootingZone && m_weaponSystem && !m_weaponSystem->isReadyToShoot()) {
        m_weaponSystem->setReadyToShoot(true);
    }

    if ((farLeft || farRight) && m_weaponSystem && m_weaponSystem->isReadyToShoot()) {
        m_weaponSystem->setReadyToShoot(false);
    }

    if (currentPos.x < cameraLeft - 400.0f) {
        float newX = cameraRight + 300.0f;
        physics->setPosition(newX, m_flightAltitude);
        if (m_weaponSystem)
            m_weaponSystem->reset();
    }

    if (!isActive()) {
        setActive(true);
    }
}
//-------------------------------------------------------------------------------------
void FalconEnemyEntity::update(float dt) {
    // Force active always
    if (!isActive()) {
        setActive(true);
    }

    EnemyEntity::update(dt);
    updateAnimation(dt);
    updateFlightPattern(dt);
    if (m_weaponSystem)
        m_weaponSystem->update(dt);

    auto* physics = getComponent<PhysicsComponent>();
    auto* render = getComponent<RenderComponent>();
    if (physics && render) {
        render->getSprite().setPosition(physics->getPosition());
    }
}
//-------------------------------------------------------------------------------------
void FalconEnemyEntity::updateAnimation(float dt) {
    m_animationTimer += dt;
    if (m_animationTimer >= m_animationSpeed) {
        switchTexture();
        m_animationTimer = 0.0f;
    }
}
//-------------------------------------------------------------------------------------
void FalconEnemyEntity::switchTexture() {
    auto* render = getComponent<RenderComponent>();
    if (!render || !m_texture1 || !m_texture2) return;

    m_currentFrame = (m_currentFrame + 1) % 2;
    render->setTexture(m_currentFrame == 0 ? *m_texture1 : *m_texture2);

    auto& sprite = render->getSprite();
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
    float currentScaleY = sprite.getScale().y;
    sprite.setScale(-abs(sprite.getScale().x), currentScaleY);
}
//-------------------------------------------------------------------------------------