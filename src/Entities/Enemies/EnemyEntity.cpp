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

//-------------------------------------------------------------------------------------
EnemyEntity::EnemyEntity(IdType id, EnemyType type, b2World& , float, float, TextureManager& textures)
    : Entity(id)
    , m_enemyType(type)
    , m_textures(textures) {}
//-------------------------------------------------------------------------------------
void EnemyEntity::setupComponents(b2World&, float x, float y, TextureManager& ) {
    addComponent<Transform>(sf::Vector2f(x, y));
    addComponent<HealthComponent>(1);
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Enemy);
}
//-------------------------------------------------------------------------------------
void EnemyEntity::update(float dt) {
    Entity::update(dt); 

    auto* physics = getComponent<PhysicsComponent>();
    auto* render = getComponent<RenderComponent>();
    auto* transform = getComponent<Transform>();

    if (physics && render && transform) {
        sf::Vector2f pos = physics->getPosition();
        transform->setPosition(pos);
        render->getSprite().setPosition(pos);

        sf::Vector2f velocity = physics->getVelocity();
        bool onGround = std::abs(velocity.y) < 0.1f;
    }
}
//-------------------------------------------------------------------------------------