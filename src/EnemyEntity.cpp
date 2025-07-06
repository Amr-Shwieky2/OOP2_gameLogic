#include "EnemyEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "MovementComponent.h"
#include "AIComponent.h"
#include "PatrolStrategy.h"
#include "FollowPlayerStrategy.h"
#include "ResourceManager.h"
#include "Constants.h"
#include <iostream>

EnemyEntity::EnemyEntity(IdType id, EnemyType type, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id)
    , m_enemyType(type)
    , m_textures(textures) {
    std::cout << "[ENEMY BASE] Constructor called for ID " << id << " type " << (int)type << std::endl;
    // DON'T call setupComponents here - let derived classes do it
}

void EnemyEntity::setupComponents(b2World&, float x, float y, TextureManager& textures) {
    std::cout << "[ENEMY BASE] setupComponents called at (" << x << ", " << y << ")" << std::endl;
    // Base enemy setup - derived classes will override
    addComponent<Transform>(sf::Vector2f(x, y));
    addComponent<HealthComponent>(1);
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Enemy);
}

// Add this to EnemyEntity class update method
void EnemyEntity::update(float dt) {
    Entity::update(dt); // Call base update

    // Sync sprite position with physics
    auto* physics = getComponent<PhysicsComponent>();
    auto* render = getComponent<RenderComponent>();
    auto* transform = getComponent<Transform>();

    if (physics && render && transform) {
        sf::Vector2f pos = physics->getPosition();
        transform->setPosition(pos);
        render->getSprite().setPosition(pos);

        // Check if enemy is on ground (simple check)
        sf::Vector2f velocity = physics->getVelocity();
        bool onGround = std::abs(velocity.y) < 0.1f;

        // Debug output every 60 frames
        static int debugFrame = 0;
        debugFrame++;
        if (debugFrame % 60 == 0) {
            std::cout << "[ENEMY " << getId() << "] Pos: (" << pos.x << ", " << pos.y
                << ") Vel: (" << velocity.x << ", " << velocity.y << ")"
                << " OnGround: " << onGround << std::endl;
        }
    }
}
