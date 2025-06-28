#include "PlayerEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "ResourceManager.h"
#include "Constants.h"

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
    physics->createCircleShape(PLAYER_RADIUS);
    physics->setPosition(x, y);

    // Add rendering
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("NormalBall.png"));
    auto& sprite = render->getSprite();
    sprite.setScale(0.1f, 0.1f); // Adjust as needed
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    // Add health
    addComponent<HealthComponent>(3); // 3 lives

    // Add collision
    auto* collision = addComponent<CollisionComponent>(CollisionComponent::CollisionType::Player);
}

void PlayerEntity::jump() {
    if (auto* physics = getComponent<PhysicsComponent>()) {
        // Check if on ground (simplified - you'll need proper ground detection)
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE);
    }
}

void PlayerEntity::moveLeft() {
    if (auto* physics = getComponent<PhysicsComponent>()) {
        auto vel = physics->getVelocity();
        physics->setVelocity(-PLAYER_MOVE_SPEED, vel.y);
    }
}

void PlayerEntity::moveRight() {
    if (auto* physics = getComponent<PhysicsComponent>()) {
        auto vel = physics->getVelocity();
        physics->setVelocity(PLAYER_MOVE_SPEED, vel.y);
    }
}

void PlayerEntity::shoot() {
    // TODO: Create projectile entity
}

void PlayerEntity::addScore(int points) {
    m_score += points;
}

void PlayerEntity::applySpeedBoost(float duration) {
    // TODO: Implement effect system
}

void PlayerEntity::applyShield(float duration) {
    if (auto* health = getComponent<HealthComponent>()) {
        health->setInvulnerable(true);
        // TODO: Add timer to remove invulnerability
    }
}