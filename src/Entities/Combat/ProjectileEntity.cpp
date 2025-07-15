#include "ProjectileEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
#include <iostream>
#include "GameSession.h"

extern GameSession* g_currentSession;
//-------------------------------------------------------------------------------------
ProjectileEntity::ProjectileEntity(IdType id, b2World& world, float x, float y,
    sf::Vector2f direction, TextureManager& textures,
    bool fromPlayer, bool withGravity)
    : Entity(id)
    , m_fromPlayer(fromPlayer)
    , m_withGravity(withGravity) {
    setupComponents(world, x, y, direction, textures, withGravity);
}
//-------------------------------------------------------------------------------------
void ProjectileEntity::setupComponents(b2World& world, float x, float y,
    sf::Vector2f direction, TextureManager& textures, bool withGravity) {
    addComponent<Transform>(sf::Vector2f(x, y));

    if (withGravity) {
        float speed = 15.0f; 
        m_velocity = sf::Vector2f(direction.x * speed, direction.y * speed);
    }

    auto* physics = addComponent<PhysicsComponent>(world, 
                                                  withGravity ? b2_dynamicBody : b2_dynamicBody);
    float projectileSize = withGravity ? 8.0f : 6.0f; 
    physics->createCircleShape(projectileSize);
    physics->setPosition(x, y);

    if (auto* body = physics->getBody()) {
        b2Fixture* fixture = body->GetFixtureList();
        if (fixture) {
            b2Filter filter;
            filter.categoryBits = 0x0002; 
            
            if (m_fromPlayer) {
                filter.maskBits = 0x000C; 
            } else {
                filter.maskBits = 0x0009;
            }
            
            fixture->SetFilterData(filter);            
            fixture->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
            
            fixture->SetRestitution(0);
            fixture->SetFriction(0.1f);
            
            fixture->SetDensity(0.5f);
        }
        
        if (withGravity) {
            body->SetGravityScale(0.8f);
            
            float speed = 15.0f;
            physics->setVelocity(direction.x * speed, direction.y * speed);
        } 
        else {
            body->SetGravityScale(0.1f);
            
            float speed = 10.0f;
            physics->setVelocity(direction.x * speed, direction.y * speed);
        }

        body->SetBullet(true);              // Fast collision detection
        body->SetSleepingAllowed(false);    // Never sleep
        body->SetAwake(true);               // Always awake
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
        body->ResetMassData();              // Update mass after changes
    }

    // Rendering
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("Bullet.png"));
    auto& sprite = render->getSprite();
    
    if (withGravity) {
        sprite.setScale(0.09f, 0.09f);
        sprite.setColor(sf::Color(255, 200, 100)); 
    } else {
        sprite.setScale(0.07f, 0.07f);
        
        if (!m_fromPlayer) {
            sprite.setColor(sf::Color(255, 100, 100)); 
        }
    }
    
    float angle = atan2(direction.y, direction.x) * 180.0f / 3.14159f;
    sprite.setRotation(angle);

    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    sprite.setPosition(x, y);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Projectile);
}
//-------------------------------------------------------------------------------------
void ProjectileEntity::update(float dt) {
    Entity::update(dt);

    m_lifetime -= dt;
    if (m_lifetime <= 0) {
        setActive(false);
        return;
    }

    auto* physics = getComponent<PhysicsComponent>();
    auto* render = getComponent<RenderComponent>();
    auto* transform = getComponent<Transform>();

    if (physics && render && transform) {
        if (m_withGravity || true) {  
            sf::Vector2f velocity = physics->getVelocity();
            
            if (std::abs(velocity.x) > 0.1f || std::abs(velocity.y) > 0.1f) {
                float angle = atan2(velocity.y, velocity.x) * 180.0f / 3.14159f;
                render->getSprite().setRotation(angle);
            }
        }
        sf::Vector2f pos = physics->getPosition();
        transform->setPosition(pos);
        render->getSprite().setPosition(pos);
        
        sf::Vector2f velocity = physics->getVelocity();
        if (velocity.x < 0.5f && velocity.y < 0.5f && 
            velocity.x > -0.5f && velocity.y > -0.5f) {
            m_stopTime += dt;
            
            if (m_stopTime > 0.2f) {
                setActive(false);
                return;
            }
        } else {
            m_stopTime = 0.0f;
        }
    }

    if (!g_currentSession) return;

    auto* playerEntity = g_currentSession->getPlayer();
    if (!playerEntity) return;

    auto* playerTransform = playerEntity->getComponent<Transform>();
    if (!playerTransform) return;

    auto* projectileTransform = getComponent<Transform>();
    if (!projectileTransform) return;

    sf::Vector2f playerPos = playerTransform->getPosition();
    sf::Vector2f projectilePos = projectileTransform->getPosition();

    float cameraLeft = playerPos.x - WINDOW_WIDTH / 2.0f;
    float cameraRight = playerPos.x + WINDOW_WIDTH / 2.0f;
    float cameraTop = playerPos.y - WINDOW_HEIGHT / 2.0f;
    float cameraBottom = playerPos.y + WINDOW_HEIGHT / 2.0f;

    const float margin = 300.0f;
    
    if (projectilePos.x < cameraLeft - margin || projectilePos.x > cameraRight + margin ||
        projectilePos.y < cameraTop - margin || projectilePos.y > cameraBottom + margin) {
        setActive(false);
    }
}
//-------------------------------------------------------------------------------------