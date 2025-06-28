#include "EnemyEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "MovementComponent.h"
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
    
    // Add physics
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(ENEMY_WIDTH * PPM, ENEMY_HEIGHT * PPM);
    physics->setPosition(x, y);
    
    // Add rendering
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("SquareEnemy.png"));
    auto& sprite = render->getSprite();
    sprite.setScale(0.1f, 0.1f);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Add movement (following player)
    auto* movement = addComponent<MovementComponent>(MovementComponent::MovementType::Following);
    movement->setSpeed(1.0f);
}