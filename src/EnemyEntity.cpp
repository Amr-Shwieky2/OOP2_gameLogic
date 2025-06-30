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

// SquareEnemyEntity implementation
SquareEnemyEntity::SquareEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : EnemyEntity(id, EnemyType::Square, world, x, y, textures) {
    std::cout << "[SQUARE ENEMY] Constructor called, now calling setupComponents" << std::endl;
    // Call setup here where virtual dispatch works correctly
    setupComponents(world, x, y, textures);
}

void SquareEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    std::cout << "\n[DEBUG] Creating SquareEnemy at position (" << x << ", " << y << ")" << std::endl;

    // Call base setup
    EnemyEntity::setupComponents(world, x, y, textures);

    // Position enemy at tile center
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f;

    std::cout << "[DEBUG] Enemy center position: (" << centerX << ", " << centerY << ")" << std::endl;

    // Update transform position
    auto* transform = getComponent<Transform>();
    if (transform) {
        transform->setPosition(centerX, centerY);
        std::cout << "[DEBUG] Transform component set successfully" << std::endl;
    }
    else {
        std::cout << "[ERROR] Failed to get Transform component!" << std::endl;
    }

    // Add physics - MAKE SURE IT'S AT THE RIGHT HEIGHT
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    if (physics) {
        // Create a box that's visible
        physics->createBoxShape(TILE_SIZE * 0.8f, TILE_SIZE * 0.8f,
            1.0f,    // density
            0.3f,    // friction - important for movement!
            0.0f);   // restitution
        physics->setPosition(centerX, centerY);
        std::cout << "[DEBUG] Physics component created at (" << centerX << ", " << centerY << ")" << std::endl;

        // Configure physics body
        if (auto* body = physics->getBody()) {
            body->SetFixedRotation(true);
            body->SetGravityScale(1.0f);

            // IMPORTANT: Set linear damping to allow movement
            body->SetLinearDamping(0.0f); // No damping

            // Ensure the body is awake
            body->SetAwake(true);

            // Add user data for debugging
            body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);

            std::cout << "[DEBUG] Physics body configured - Awake: " << body->IsAwake()
                << " Type: " << body->GetType() << std::endl;
        }
    }
    else {
        std::cout << "[ERROR] Failed to create Physics component!" << std::endl;
    }

    // Add rendering with better visibility
    auto* render = addComponent<RenderComponent>();
    if (render) {
        try {
            render->setTexture(textures.getResource("SquareEnemy.png"));
            auto& sprite = render->getSprite();

            // Get texture size for debugging
            sf::Vector2u texSize = sprite.getTexture()->getSize();
            std::cout << "[DEBUG] Enemy texture size: " << texSize.x << "x" << texSize.y << std::endl;

            // Make enemy larger and more visible
            sprite.setScale(0.3f, 0.3f); // Increased from 0.1f
            sprite.setColor(sf::Color(255, 100, 100)); // Red tint for visibility

            auto bounds = sprite.getLocalBounds();
            sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
            sprite.setPosition(centerX, centerY); // Set initial sprite position

            std::cout << "[DEBUG] Render component configured - Scale: 0.3, Color: Red tint" << std::endl;
            std::cout << "[DEBUG] Sprite bounds: " << bounds.width << "x" << bounds.height << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "[ERROR] Failed to load enemy texture: " << e.what() << std::endl;
        }
    }
    else {
        std::cout << "[ERROR] Failed to create Render component!" << std::endl;
    }

    // Add AI with follow strategy by default
    auto* ai = addComponent<AIComponent>(std::make_unique<PatrolStrategy>(300.0f, 80.0f)); // Patrol 300 pixels at speed 80
    if (ai) {
        std::cout << "[DEBUG] AI component added with FollowPlayer strategy" << std::endl;
    }
    else {
        std::cout << "[ERROR] Failed to create AI component!" << std::endl;
    }

    std::cout << "[DEBUG] SquareEnemy setup complete for ID: " << getId() << std::endl;
}