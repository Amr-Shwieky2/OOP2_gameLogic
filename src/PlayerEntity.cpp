// ==========================================
// PlayerEntity.cpp - Enhanced Implementation
// ==========================================

#include "PlayerEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
#include <iostream>

PlayerEntity::PlayerEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id)
    , m_textures(textures) {
    setupComponents(world, x, y, textures);
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

    // Update effect timers
    if (m_speedBoostTimer > 0.0f) {
        m_speedBoostTimer -= dt;
    }
    if (m_shieldTimer > 0.0f) {
        m_shieldTimer -= dt;
        // Update shield visual effect
        auto* health = getComponent<HealthComponent>();
        if (health) {
            health->setInvulnerable(m_shieldTimer > 0.0f);
        }
    }

    // Update visuals and physics
    updateVisuals();
    updatePhysics();
}

void PlayerEntity::handleInput(const InputService& input) {
    auto* physics = getComponent<PhysicsComponent>();
    if (!physics) return;

    float moveSpeed = PLAYER_MOVE_SPEED;

    // Apply speed boost effect
    if (m_speedBoostTimer > 0.0f) {
        moveSpeed *= 1.5f;
    }

    // Handle left/right movement
    if (input.isKeyDown(sf::Keyboard::Left)) {
        auto vel = physics->getVelocity();
        physics->setVelocity(-moveSpeed, vel.y);
    }
    else if (input.isKeyDown(sf::Keyboard::Right)) {
        auto vel = physics->getVelocity();
        physics->setVelocity(moveSpeed, vel.y);
    }
    else {
        // Stop horizontal movement when no input
        auto vel = physics->getVelocity();
        physics->setVelocity(0, vel.y);
    }

    // Handle jumping
    if (input.isKeyPressed(sf::Keyboard::Up) && isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE);
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
    if (auto* physics = getComponent<PhysicsComponent>()) {
        float moveSpeed = PLAYER_MOVE_SPEED;
        if (m_speedBoostTimer > 0.0f) moveSpeed *= 1.5f;

        auto vel = physics->getVelocity();
        physics->setVelocity(-moveSpeed, vel.y);
    }
}

void PlayerEntity::moveRight() {
    if (auto* physics = getComponent<PhysicsComponent>()) {
        float moveSpeed = PLAYER_MOVE_SPEED;
        if (m_speedBoostTimer > 0.0f) moveSpeed *= 1.5f;

        auto vel = physics->getVelocity();
        physics->setVelocity(moveSpeed, vel.y);
    }
}

void PlayerEntity::shoot() {
    // TODO: Create projectile entity and add to EntityManager
    // This will require access to the EntityManager and EntityFactory
    std::cout << "Player shoot (TODO: implement projectile creation)" << std::endl;
}

void PlayerEntity::addScore(int points) {
    m_score += points;
}

void PlayerEntity::applySpeedBoost(float duration) {
    m_speedBoostTimer = duration;
    std::cout << "Speed boost applied for " << duration << " seconds" << std::endl;
}

void PlayerEntity::applyShield(float duration) {
    m_shieldTimer = duration;

    // Update visual
    auto* render = getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(255, 255, 255, 128)); // Semi-transparent
    }

    std::cout << "Shield applied for " << duration << " seconds" << std::endl;
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

        // Update texture based on effects
        if (m_shieldTimer > 0.0f) {
            render->setTexture(m_textures.getResource("TransparentBall.png"));
        }
        else {
            render->setTexture(m_textures.getResource("NormalBall.png"));
        }
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
