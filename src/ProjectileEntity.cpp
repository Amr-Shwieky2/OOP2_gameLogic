// ProjectileEntity.cpp
#include "ProjectileEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
#include <iostream>
#include "GameSession.h"

extern GameSession* g_currentSession;

ProjectileEntity::ProjectileEntity(IdType id, b2World& world, float x, float y,
    sf::Vector2f direction, TextureManager& textures,
    bool fromPlayer, bool withGravity)
    : Entity(id)
    , m_fromPlayer(fromPlayer)
    , m_withGravity(withGravity) {
    setupComponents(world, x, y, direction, textures, withGravity);
}

void ProjectileEntity::setupComponents(b2World& world, float x, float y,
    sf::Vector2f direction, TextureManager& textures, bool withGravity) {
    // Add transform
    addComponent<Transform>(sf::Vector2f(x, y));

    // For gravity projectiles, we'll need to track velocity manually
    if (withGravity) {
        // Store initial velocity (for manual gravity calculations)
        float speed = 15.0f; // Higher initial speed for gravity projectiles
        m_velocity = sf::Vector2f(direction.x * speed, direction.y * speed);
    }

    // Kinematic bodies - use dynamic for gravity-affected ones
    auto* physics = addComponent<PhysicsComponent>(world, 
                                                  withGravity ? b2_dynamicBody : b2_dynamicBody);
    float projectileSize = withGravity ? 8.0f : 6.0f; // Bigger projectiles for gravity shots
    physics->createCircleShape(projectileSize);
    physics->setPosition(x, y);

    if (auto* body = physics->getBody()) {
        b2Fixture* fixture = body->GetFixtureList();
        if (fixture) {
            b2Filter filter;
            filter.categoryBits = 0x0002; // Projectile category
            
            if (m_fromPlayer) {
                // Player projectiles collide with ground (0x0008) and enemies (0x0004)
                filter.maskBits = 0x000C; 
            } else {
                // Enemy projectiles collide with ground (0x0008) and player (0x0001)
                filter.maskBits = 0x0009;
            }
            
            fixture->SetFilterData(filter);
            
            // Set the fixture user data to point back to this entity
            fixture->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
            
            // Make projectiles bounce less off surfaces
            fixture->SetRestitution(0.2f);
            fixture->SetFriction(0.1f);
            
            // Lower density for better physics
            fixture->SetDensity(0.5f);
        }
        
        // Set gravity scale for bodies
        if (withGravity) {
            // Apply reduced gravity (80% of normal)
            body->SetGravityScale(0.8f);
            
            // Initial velocity - higher for gravity projectiles
            float speed = 15.0f;
            physics->setVelocity(direction.x * speed, direction.y * speed);
        } 
        else {
            // Even non-gravity projectiles should have minimal gravity to prevent
            // them from passing through thin platforms
            body->SetGravityScale(0.1f);
            
            // Regular projectile speed
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
    // Choose a different texture for gravity projectiles if you want
    render->setTexture(textures.getResource("Bullet.png"));
    auto& sprite = render->getSprite();
    
    // Make gravity projectiles slightly larger and differently colored
    if (withGravity) {
        sprite.setScale(0.09f, 0.09f);
        sprite.setColor(sf::Color(255, 200, 100)); // Orange-ish
    } else {
        sprite.setScale(0.07f, 0.07f);
        
        // Different color for enemy vs player projectiles
        if (!m_fromPlayer) {
            sprite.setColor(sf::Color(255, 100, 100)); // Reddish for enemy
        }
    }
    
    // Set rotation based on direction
    float angle = atan2(direction.y, direction.x) * 180.0f / 3.14159f;
    sprite.setRotation(angle);

    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Set initial position
    sprite.setPosition(x, y);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Projectile);
}

void ProjectileEntity::update(float dt) {
    Entity::update(dt);

    // Update lifetime
    m_lifetime -= dt;
    if (m_lifetime <= 0) {
        setActive(false);
        return;
    }

    // Update position
    auto* physics = getComponent<PhysicsComponent>();
    auto* render = getComponent<RenderComponent>();
    auto* transform = getComponent<Transform>();

    if (physics && render && transform) {
        // For gravity projectiles, we want to update the rotation to follow the trajectory
        if (m_withGravity || true) {  // Always update rotation for all projectiles
            sf::Vector2f velocity = physics->getVelocity();
            
            // Update sprite rotation to match current velocity direction
            if (std::abs(velocity.x) > 0.1f || std::abs(velocity.y) > 0.1f) {
                float angle = atan2(velocity.y, velocity.x) * 180.0f / 3.14159f;
                render->getSprite().setRotation(angle);
            }
        }
        
        // Update position
        sf::Vector2f pos = physics->getPosition();
        transform->setPosition(pos);
        render->getSprite().setPosition(pos);
        
        // Check if projectile has stopped moving (hit ground)
        sf::Vector2f velocity = physics->getVelocity();
        if (velocity.x < 0.5f && velocity.y < 0.5f && 
            velocity.x > -0.5f && velocity.y > -0.5f) {
            m_stopTime += dt;
            
            if (m_stopTime > 0.2f) {
                // Projectile has been still for a while, destroy it
                setActive(false);
                return;
            }
        } else {
            m_stopTime = 0.0f;
        }
    }

    // Check if projectile is off-screen relative to the camera view and destroy it
    if (!g_currentSession) return;

    auto* playerEntity = g_currentSession->getPlayer();
    if (!playerEntity) return;

    auto* playerTransform = playerEntity->getComponent<Transform>();
    if (!playerTransform) return;

    auto* projectileTransform = getComponent<Transform>();
    if (!projectileTransform) return;

    sf::Vector2f playerPos = playerTransform->getPosition();
    sf::Vector2f projectilePos = projectileTransform->getPosition();

    // Calculate camera bounds based on player position
    float cameraLeft = playerPos.x - WINDOW_WIDTH / 2.0f;
    float cameraRight = playerPos.x + WINDOW_WIDTH / 2.0f;
    float cameraTop = playerPos.y - WINDOW_HEIGHT / 2.0f;
    float cameraBottom = playerPos.y + WINDOW_HEIGHT / 2.0f;

    // Add extra margin (300 pixels in each direction)
    const float margin = 300.0f;
    
    // Destroy if way off screen relative to camera
    if (projectilePos.x < cameraLeft - margin || projectilePos.x > cameraRight + margin ||
        projectilePos.y < cameraTop - margin || projectilePos.y > cameraBottom + margin) {
        setActive(false);
    }
}