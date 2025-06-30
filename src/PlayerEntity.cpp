// PlayerEntity.cpp - Implementation with State Pattern
#include "PlayerEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
#include "PlayerState.h"
#include "NormalState.h"
#include "ShieldedState.h"
#include "BoostedState.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include "GameSession.h"
#include "ProjectileEntity.h"
#include <memory>
#include <iostream>
#include <cmath>

PlayerEntity::PlayerEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id)
    , m_textures(textures)
    , m_world(world) {
    setupComponents(world, x, y, textures);

    // Start in normal state
    m_currentState = NormalState::getInstance();
    m_currentState->enter(*this);
}

void PlayerEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    // Add transform
    auto* transform = addComponent<Transform>(sf::Vector2f(x, y));

    // Add physics
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createCircleShape(PLAYER_RADIUS * PPM);
    physics->setPosition(x, y);

    // Add rendering
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("NormalBall.png"));
    auto& sprite = render->getSprite();

    // Scale to desired size
    float desiredDiameter = PLAYER_RADIUS * 2 * PPM;
    sf::Vector2u textureSize = sprite.getTexture()->getSize();
    float scaleX = desiredDiameter / textureSize.x;
    float scaleY = desiredDiameter / textureSize.y;
    sprite.setScale(scaleX, scaleY);
    sprite.setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);

    // Add health
    addComponent<HealthComponent>(3); // 3 lives

    // Add collision
    auto* collision = addComponent<CollisionComponent>(CollisionComponent::CollisionType::Player);
}

void PlayerEntity::update(float dt) {
    // Call base update (updates all components)
    Entity::update(dt);

    // Update current state
    if (m_currentState) {
        m_currentState->update(*this, dt);
    }

    // Apply rolling rotation based on movement
    applyRollRotation(dt);

    // Update visuals and physics
    updateVisuals();
    updatePhysics();
}

void PlayerEntity::changeState(PlayerState* newState) {
    if (m_currentState == newState) return;

    std::string oldStateName = m_currentState ? m_currentState->getName() : "None";
    std::string newStateName = newState ? newState->getName() : "None";

    std::cout << "[Player] State change: " << oldStateName
        << " -> " << newStateName << std::endl;

    if (m_currentState) {
        m_currentState->exit(*this);
    }

    m_currentState = newState;

    if (m_currentState) {
        m_currentState->enter(*this);
    }

    // Publish state changed event
    EventSystem::getInstance().publish(
        PlayerStateChangedEvent(oldStateName, newStateName)
    );
}
void PlayerEntity::handleInput(const InputService& input) {
    if (m_currentState) {
        m_currentState->handleInput(*this, input);
    }
}

void PlayerEntity::jump() {
    if (auto* physics = getComponent<PhysicsComponent>()) {
        if (isOnGround()) {
            physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE);
        }
    }
}

void PlayerEntity::moveLeft() {
    // Movement is now handled by states
}

void PlayerEntity::moveRight() {
    // Movement is now handled by states
}

void PlayerEntity::shoot() {
    extern int g_nextEntityId;
    extern GameSession* g_currentSession;

    if (!g_currentSession) {
        std::cerr << "[PlayerEntity] No active game session for shooting" << std::endl;
        return;
    }

    // Determine shoot direction based on current velocity
    sf::Vector2f dir = getVelocity().x >= 0 ? sf::Vector2f(1.f, 0.f) : sf::Vector2f(-1.f, 0.f);
    sf::Vector2f pos = getPosition();

    auto projectile = std::make_unique<ProjectileEntity>(
        g_nextEntityId++, m_world, pos.x, pos.y, dir, m_textures, true);
    g_currentSession->spawnEntity(std::move(projectile));
}

void PlayerEntity::addScore(int points) {
    int oldScore = m_score;
    m_score += points;

    // Publish score changed event
    EventSystem::getInstance().publish(
        ScoreChangedEvent(m_score, points)
    );
}

void PlayerEntity::applySpeedBoost(float duration) {
    changeState(BoostedState::getInstance());
}

void PlayerEntity::applyShield(float duration) {
    changeState(ShieldedState::getInstance());
}

sf::Vector2f PlayerEntity::getPosition() const {
    auto* transform = getComponent<Transform>();
    return transform ? transform->getPosition() : sf::Vector2f(0, 0);
}

sf::Vector2f PlayerEntity::getVelocity() const {
    auto* physics = getComponent<PhysicsComponent>();
    return physics ? physics->getVelocity() : sf::Vector2f(0, 0);
}

bool PlayerEntity::isOnGround() const {
    return m_groundContacts > 0;
}

void PlayerEntity::updateVisuals() {
    auto* render = getComponent<RenderComponent>();
    auto* transform = getComponent<Transform>();

    if (render && transform) {
        // Update sprite position
        render->getSprite().setPosition(transform->getPosition());
    }
}

void PlayerEntity::updatePhysics() {
    auto* physics = getComponent<PhysicsComponent>();
    auto* transform = getComponent<Transform>();

    if (physics && transform) {
        // Sync transform with physics
        transform->setPosition(physics->getPosition());

        // Ground detection (simplified)
        sf::Vector2f vel = physics->getVelocity();
        if (std::abs(vel.y) < 0.1f) {
            m_groundContacts = 1; // Simplified ground detection
        }
        else {
            m_groundContacts = 0;
        }
    }
}

void PlayerEntity::applyRollRotation(float dt) {
    auto* render = getComponent<RenderComponent>();
    auto* physics = getComponent<PhysicsComponent>();

    if (render && physics) {
        float vx = physics->getVelocity().x;
        float deltaAngle = vx * dt * PLAYER_SPIN_RATE;
        render->getSprite().rotate(deltaAngle);
    }
}