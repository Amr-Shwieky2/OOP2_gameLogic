// SmartEnemyEntity.cpp
#include "SmartEnemyEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "AIComponent.h"
#include "PatrolStrategy.h"
#include "FollowPlayerStrategy.h"
#include "GuardStrategy.h"
#include "PlayerEntity.h"
#include "GameSession.h"
#include <iostream>
#include <Constants.h>

SmartEnemyEntity::SmartEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : EnemyEntity(id, EnemyType::Smart, world, x, y, textures) {
    std::cout << "[SMART ENEMY] Constructor called, now calling setupComponents" << std::endl;
    // Call setup here where virtual dispatch works correctly
    setupComponents(world, x, y, textures);
}

void SmartEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    EnemyEntity::setupComponents(world, x, y, textures);

    // Add physics
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(ENEMY_WIDTH * PPM * 1.2f, ENEMY_HEIGHT * PPM * 1.2f); // Slightly larger
    physics->setPosition(x, y);

    // Add rendering - different color/texture for smart enemy
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("SquareEnemy.png")); // Could use different texture
    auto& sprite = render->getSprite();

    sprite.setScale(0.12f, 0.12f); // Slightly larger
    sprite.setColor(sf::Color(255, 200, 200)); // Reddish tint
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    std::cout << "x: " << sprite.getPosition().x << ", y:" << sprite.getPosition().y;
    // Start with patrol strategy
    auto* ai = addComponent<AIComponent>(std::make_unique<PatrolStrategy>(150.0f, 75.0f));

    // More health for smart enemy
    auto* health = getComponent<HealthComponent>();
    if (health) {
        health->setHealth(3); // 3 hits to kill
    }
}

void SmartEnemyEntity::update(float dt) {
    EnemyEntity::update(dt);

    m_strategyTimer += dt;

    // Update strategy every 2 seconds
    if (m_strategyTimer >= 2.0f) {
        updateStrategy();
        m_strategyTimer = 0.0f;
    }
    
}

void SmartEnemyEntity::updateStrategy() {
    auto* ai = getComponent<AIComponent>();
    if (!ai || !ai->getStrategy()) {
        std::cout << "[SMART ENEMY] No AI component or strategy!" << std::endl;
        return;
    }

    auto* transform = getComponent<Transform>();
    if (!transform) return;

    // Check if we have a player target
    PlayerEntity* player = nullptr;
    if (ai->getStrategy()) {
        // Assume player is set in AI component's target
        // For now, we'll just cycle through strategies
    }

    // Cycle through strategies for demonstration
    switch (m_currentStrategy) {
    case CurrentStrategy::Patrol:
        std::cout << "[SmartEnemy " << getId() << "] Switching to Follow strategy" << std::endl;
        ai->setStrategy(std::make_unique<FollowPlayerStrategy>(120.0f, 400.0f));
        m_currentStrategy = CurrentStrategy::Follow;
        break;

    case CurrentStrategy::Follow:
        std::cout << "[SmartEnemy " << getId() << "] Switching to Guard strategy" << std::endl;
        ai->setStrategy(std::make_unique<GuardStrategy>(200.0f, 150.0f));
        m_currentStrategy = CurrentStrategy::Guard;
        break;

    case CurrentStrategy::Guard:
        std::cout << "[SmartEnemy " << getId() << "] Switching to Patrol strategy" << std::endl;
        ai->setStrategy(std::make_unique<PatrolStrategy>(200.0f, 80.0f));
        m_currentStrategy = CurrentStrategy::Patrol;
        break;
    }
}