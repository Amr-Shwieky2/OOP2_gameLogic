#include "SquareEnemyEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "AIComponent.h"
#include "PatrolStrategy.h"
#include "GameSession.h"
#include "Constants.h"
#include <iostream>
#include <random>
#include <MovementComponent.h>
#include <SmartEnemyEntity.h>

extern GameSession* g_currentSession;
//-------------------------------------------------------------------------------------
SquareEnemyEntity::SquareEnemyEntity(IdType id, b2World& world, float x, float y,
    TextureManager& textures, SizeType size)
    : EnemyEntity(id, EnemyType::Square, world, x, y, textures)
    , m_sizeType(size) {
    setupComponents(world, x, y, textures);
}
//-------------------------------------------------------------------------------------
void SquareEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f;

    auto* transform = addComponent<Transform>(sf::Vector2f(centerX, centerY));
    transform->setPosition(centerX, centerY);

    float sizeMultiplier = getSizeMultiplier();
    float physicsSize = TILE_SIZE * sizeMultiplier;

    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(physicsSize, physicsSize, 1.0f, 0.3f, 0.0f);
    physics->setPosition(centerX, centerY);

    if (auto* body = physics->getBody()) {
        body->SetFixedRotation(true);
        body->SetGravityScale(1.0f);
        body->SetLinearDamping(0.0f);
        body->SetAwake(true);
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("SquareEnemy2.png"));
    auto& sprite = render->getSprite();

    float renderScale = sizeMultiplier * 0.3f;
    sprite.setScale(renderScale, renderScale);

    sf::Color enemyColor = (m_sizeType == SizeType::Large)
        ? sf::Color(255, 100, 100)
        : sf::Color(255, 200, 100);

    sprite.setColor(enemyColor);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite.setPosition(centerX, centerY);

    addComponent<HealthComponent>(static_cast<int>(getHealthForSize()));
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Enemy);
    addComponent<MovementComponent>();

    float patrolDistance = 200.0f * sizeMultiplier;
    float speed = getSpeedForSize();
    addComponent<AIComponent>(std::make_unique<PatrolStrategy>(patrolDistance, speed));
}
//-------------------------------------------------------------------------------------
void SquareEnemyEntity::onDeath(Entity* killer) {
    if (killer && canSplit()) {
        if (auto* transform = getComponent<Transform>()) {
            spawnSplitEnemies(transform->getPosition());
        }
    }
    EnemyEntity::onDeath(killer);
}
//-------------------------------------------------------------------------------------
void SquareEnemyEntity::spawnSplitEnemies(const sf::Vector2f& deathPosition) {
    if (!g_currentSession) return;

    auto* physics = getComponent<PhysicsComponent>();
    if (!physics || !physics->getBody()) return;

    b2World& world = *physics->getBody()->GetWorld();

    float baseDistance = 150.0f;  

    for (int i = 0; i < 2; ++i) {
        float direction = (i == 0) ? -1.0f : 1.0f;  
        sf::Vector2f spawnOffset(direction * baseDistance, 0.0f);  
        sf::Vector2f spawnPos = deathPosition + spawnOffset;
        spawnPos.y = std::max(spawnPos.y, 100.0f);  

        try {
            auto smallEnemy = std::make_unique<SmartEnemyEntity>(
                g_currentSession->getEntityManager().generateId(),
                world,
                spawnPos.x - TILE_SIZE / 2.f,
                spawnPos.y - TILE_SIZE / 2.f,
                getTextures()
            );

            if (auto* smallPhysics = smallEnemy->getComponent<PhysicsComponent>()) {
                sf::Vector2f velocity(direction * 2.0f, 0.0f);  
                smallPhysics->setVelocity(velocity.x, velocity.y);
            }

            g_currentSession->spawnEntity(std::move(smallEnemy));
        }
        catch (const std::exception& e) {
            std::cerr << "[SQUARE ENEMY] Error spawning smart enemy: " << e.what() << std::endl;
        }
    }
}
//-------------------------------------------------------------------------------------
float SquareEnemyEntity::getSizeMultiplier() const {
    return (m_sizeType == SizeType::Large) ? LARGE_SIZE : SMALL_SIZE;
}
//-------------------------------------------------------------------------------------
float SquareEnemyEntity::getHealthForSize() const {
    return (m_sizeType == SizeType::Large) ? 2.0f : 1.0f;
}
//-------------------------------------------------------------------------------------
float SquareEnemyEntity::getSpeedForSize() const {
    return (m_sizeType == SizeType::Large) ? 60.0f : 120.0f;
}
//-------------------------------------------------------------------------------------