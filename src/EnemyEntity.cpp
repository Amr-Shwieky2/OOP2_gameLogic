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

EnemyEntity::EnemyEntity(IdType id, EnemyType type, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id)
    , m_enemyType(type)
    , m_textures(textures) {
    setupComponents(world, x, y, textures);
}

void EnemyEntity::setupComponents(b2World&, float x, float y, TextureManager& textures) {
    // Base enemy setup - derived classes will override
    addComponent<Transform>(sf::Vector2f(x, y));
    addComponent<HealthComponent>(1);
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Enemy);
}

// SquareEnemyEntity implementation
SquareEnemyEntity::SquareEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : EnemyEntity(id, EnemyType::Square, world, x, y, textures) {
}

void SquareEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    // Call base setup
    EnemyEntity::setupComponents(world, x, y, textures);

    // Position enemy at tile center
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f;

    // Update transform position
    auto* transform = getComponent<Transform>();
    if (transform) {
        transform->setPosition(centerX, centerY);
    }

    // Add physics with larger size for visibility
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(TILE_SIZE * 0.8f, TILE_SIZE * 0.8f); // 80% of tile size
    physics->setPosition(centerX, centerY);

    // Prevent falling through ground
    if (auto* body = physics->getBody()) {
        body->SetFixedRotation(true);
        body->SetGravityScale(1.0f); // Normal gravity
    }

    // Add rendering with better visibility
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("SquareEnemy.png"));
    auto& sprite = render->getSprite();

    // Make enemy larger and more visible
    sprite.setScale(0.3f, 0.3f); // Increased from 0.1f
    sprite.setColor(sf::Color(255, 100, 100)); // Red tint for visibility

    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    // Add AI with follow strategy by default
    auto* ai = addComponent<AIComponent>(std::make_unique<FollowPlayerStrategy>(100.0f, 500.0f)); // Increased detection range
}