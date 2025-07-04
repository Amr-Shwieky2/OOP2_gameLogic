﻿#include "GameCollisionSetup.h"
#include "MultiMethodCollisionSystem.h"
#include "PlayerEntity.h"
#include "EnemyEntity.h"
#include "GiftEntity.h"
#include "CoinEntity.h"
#include "ProjectileEntity.h"
#include "EntityFactory.h"
#include "HealthComponent.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "GroundEntity.h"
#include "SeaEntity.h"
#include "FlagEntity.h"
#include "CactusEntity.h"
#include "BoxEntity.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include <iostream>
#include <SmartEnemyEntity.h>
#include <FalconEnemyEntity.h>
#include <MagneticState.h>
#include <HeadwindState.h>
#include <ReversedState.h>
#include <Constants.h>

// For entity ID generation
// This needs external linkage so other modules (like SurpriseBoxManager)
// can reference it. Previously it was declared static which limited its
// visibility and caused linker errors.
int g_nextEntityId = 1;

void setupGameCollisionHandlers(MultiMethodCollisionSystem& collisionSystem) {

    // Player vs Coin
    // In GameCollisionSetup.cpp, fix the Player vs Coin handler:
    collisionSystem.registerHandler<PlayerEntity, CoinEntity>(
        [](PlayerEntity& player, CoinEntity& coin) {
            if (!coin.isActive()) return;

            player.addScore(10);
            coin.onCollect(&player);

            // Track coins properly - don't use static variable
            // Instead, publish event with incremental count
            EventSystem::getInstance().publish(
                CoinCollectedEvent(player.getId(), 1)  // Just send 1 for increment
            );

            std::cout << "Player collected coin! Score: " << player.getScore() << std::endl;
        }
    );

    // Player vs Enemy
    collisionSystem.registerHandler<PlayerEntity, SquareEnemyEntity>(
        [](PlayerEntity& player, SquareEnemyEntity& enemy) {
            if (!enemy.isActive()) return;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* enemyHealth = enemy.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();
            auto* enemyPhysics = enemy.getComponent<PhysicsComponent>();

            if (!playerHealth || !enemyHealth) return;

            // Check if player is jumping on enemy
            sf::Vector2f playerPos = playerPhysics->getPosition();
            sf::Vector2f enemyPos = enemyPhysics->getPosition();

            if (playerPos.y < enemyPos.y - 20.0f) { // Player is above enemy
                // Player kills enemy
                enemyHealth->takeDamage(999);
                enemy.setActive(false);

                // Bounce player
                playerPhysics->applyImpulse(0, -5.0f);
                player.addScore(100);

                std::cout << "Player killed enemy!" << std::endl;
            }
            else {
                // Enemy hurts player
                if (!playerHealth->isInvulnerable()) {
                    playerHealth->takeDamage(1);
                    std::cout << "Player took damage! Health: " << playerHealth->getHealth() << std::endl;

                    // Knockback
                    float knockbackDir = (playerPos.x > enemyPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 3.0f, -2.0f);
                }
            }
        }
    );

    // Player vs Gift
    collisionSystem.registerHandler<PlayerEntity, GiftEntity>(
        [](PlayerEntity& player, GiftEntity& gift) {
            if (!gift.isActive() || gift.isCollected()) return;

            std::cout << "[Collision] Player collecting gift type: "
                << static_cast<int>(gift.getGiftType()) << std::endl;

            switch (gift.getGiftType()) {
            case GiftEntity::GiftType::LifeHeart: {
                auto* health = player.getComponent<HealthComponent>();
                if (health) {
                    health->heal(1);
                    std::cout << "Player collected Life Heart! Health: "
                        << health->getHealth() << "/" << health->getMaxHealth() << std::endl;
                }
                break;
            }

            case GiftEntity::GiftType::SpeedBoost:
                player.applySpeedBoost(8.0f);  // 8 seconds
                std::cout << "Player collected Speed Boost!" << std::endl;
                break;

            case GiftEntity::GiftType::Shield:
                player.applyShield(7.0f);  // 7 seconds
                std::cout << "Player collected Shield!" << std::endl;
                break;

            case GiftEntity::GiftType::RareCoin:
                player.addScore(50);
                std::cout << "Player collected Rare Coin! +50 points" << std::endl;
                break;

            case GiftEntity::GiftType::ReverseMovement:
                // BAD GIFT - Reverses player controls!
                player.changeState(ReversedState::getInstance());
                std::cout << "[WARNING] Player collected Reverse Movement! Controls inverted!" << std::endl;

                // Notify player this is bad
                EventSystem::getInstance().publish(
                    PlayerStateChangedEvent("Normal", "Reversed")
                );
                break;

            case GiftEntity::GiftType::HeadwindStorm:
                // BAD GIFT - Slows player movement!
                player.changeState(HeadwindState::getInstance());
                std::cout << "[WARNING] Player collected Headwind Storm! Movement slowed!" << std::endl;

                // Notify player this is bad
                EventSystem::getInstance().publish(
                    PlayerStateChangedEvent("Normal", "Headwind")
                );
                break;

            case GiftEntity::GiftType::Magnetic:
                // GOOD GIFT - Attracts coins
                player.changeState(MagneticState::getInstance());
                std::cout << "Player collected Magnetic! Coins will be attracted!" << std::endl;

                EventSystem::getInstance().publish(
                    PlayerStateChangedEvent("Normal", "Magnetic")
                );
                break;
            }

            gift.collect();

            // Publish gift collected event
            EventSystem::getInstance().publish(
                ItemCollectedEvent(player.getId(), gift.getId(), ItemCollectedEvent::ItemType::Gift)
            );
        }
    );

    // Player vs Sea 
    collisionSystem.registerHandler<PlayerEntity, SeaEntity>(
        [](PlayerEntity& player, SeaEntity& sea) {

            auto* health = player.getComponent<HealthComponent>();
            auto* physics = player.getComponent<PhysicsComponent>();
            auto* render = player.getComponent<RenderComponent>();

            if (health && !health->isInvulnerable()) {
                health->setHealth(0);

                if (render) {
                    render->getSprite().setColor(sf::Color(100, 150, 255, 180));
                    render->getSprite().rotate(45.0f);
                }

                if (physics) {
                    physics->setVelocity(0, 0);
                    if (auto* body = physics->getBody()) {
                        body->SetGravityScale(0);
                    }
                }
                EventSystem::getInstance().publish(
                    PlayerDiedEvent(player.getId())
                );
            }
        }
    );

    collisionSystem.registerHandler<PlayerEntity, CactusEntity>(
        [](PlayerEntity& player, CactusEntity& cactus) {
            auto* health = player.getComponent<HealthComponent>();
            if (health && !health->isInvulnerable() && player.canTakeDamage()) {

                health->takeDamage(1);
                player.startDamageCooldown();

                // Knockback
                auto* playerPhysics = player.getComponent<PhysicsComponent>();
                auto* cactusTransform = cactus.getComponent<Transform>();
                if (playerPhysics && cactusTransform) {
                    sf::Vector2f playerPos = playerPhysics->getPosition();
                    sf::Vector2f cactusPos = cactusTransform->getPosition();
                    float knockbackDir = (playerPos.x > cactusPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 4.0f, -2.0f);
                }
            }
        }
    );

    // Player vs Flag (level complete)
    collisionSystem.registerHandler<PlayerEntity, FlagEntity>(
        [](PlayerEntity& player, FlagEntity& flag) {
            if (flag.isCompleted()) {
                return; 
            }
            std::cout << "[Collision] Player reached the flag!" << std::endl;

            flag.setCompleted(true);

            EventSystem::getInstance().publish(
                FlagReachedEvent(player.getId(), flag.getId(), "current_level")
            );
            player.addScore(500); 

            std::cout << "Level Complete! Player reached the flag!" << std::endl;
        }
    );

    // Projectile (from player) vs Enemy
    collisionSystem.registerHandler<ProjectileEntity, EnemyEntity>(
        [](ProjectileEntity& proj, EnemyEntity& enemy) {
            if (!proj.isFromPlayer() || !enemy.isActive()) return;

            auto* health = enemy.getComponent<HealthComponent>();
            if (health) {
                health->takeDamage(1);
                if (!health->isAlive()) {
                    enemy.setActive(false);
                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(enemy.getId(), proj.getId()));
                }
            }

            proj.setActive(false);
        }
    );

    // Enemy Projectile vs Player 
    collisionSystem.registerHandler<ProjectileEntity, PlayerEntity>(
        [](ProjectileEntity& proj, PlayerEntity& player) {
            // Only handle enemy projectiles hitting player
            if (proj.isFromPlayer() || !proj.isActive()) return;

            std::cout << "[Collision] Enemy projectile hit player!" << std::endl;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();

            if (playerHealth && !playerHealth->isInvulnerable()) {
                // Player takes damage
                playerHealth->takeDamage(1);

                // Add knockback effect
                if (playerPhysics) {
                    // Knockback upward and slightly away from projectile
                    playerPhysics->applyImpulse(0.5f, -1.5f);
                }

                // Visual feedback - flash player red briefly
                auto* playerRender = player.getComponent<RenderComponent>();
                if (playerRender) {
                    playerRender->getSprite().setColor(sf::Color(255, 150, 150)); // Light red tint
                }

                // Check if player died
                if (!playerHealth->isAlive()) {
                    std::cout << "[GAME] Player killed by enemy projectile!" << std::endl;
                    EventSystem::getInstance().publish(
                        PlayerDiedEvent(player.getId())
                    );
                }
            }
            else if (playerHealth && playerHealth->isInvulnerable()) {
                std::cout << "[Shield] Player is protected by shield!" << std::endl;
            }

            // Destroy the projectile
            proj.setActive(false);
        }
    );

    // Player vs Falcon direct contact 
    collisionSystem.registerHandler<PlayerEntity, FalconEnemyEntity>(
        [](PlayerEntity& player, FalconEnemyEntity& falcon) {
            if (!falcon.isActive()) return;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();
            auto* falconPhysics = falcon.getComponent<PhysicsComponent>();

            if (!playerHealth || !playerPhysics || !falconPhysics) return;

            std::cout << "[Collision] Player touched flying falcon!" << std::endl;

            // Check if player is jumping on falcon (attacking from above)
            sf::Vector2f playerPos = playerPhysics->getPosition();
            sf::Vector2f falconPos = falconPhysics->getPosition();

            if (playerPos.y < falconPos.y - 30.0f) {
                // Player is above falcon - kill falcon
                auto* falconHealth = falcon.getComponent<HealthComponent>();
                if (falconHealth) {
                    falconHealth->takeDamage(999); // Instant kill
                    falcon.setActive(false);

                    // Bounce player upward
                    playerPhysics->applyImpulse(0, -4.0f);
                    player.addScore(200); // Higher score for flying enemy

                    std::cout << "Player defeated flying falcon! +200 points" << std::endl;

                    // Publish enemy killed event
                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(falcon.getId(), player.getId())
                    );
                }
            }
            else {
                // Falcon hurts player (side contact)
                if (!playerHealth->isInvulnerable()) {
                    playerHealth->takeDamage(1);
                    std::cout << "Player hit by flying falcon! Health: "
                        << playerHealth->getHealth() << std::endl;

                    // Strong knockback from flying enemy
                    float knockbackDir = (playerPos.x > falconPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 4.0f, -3.0f);

                    // Visual feedback
                    auto* playerRender = player.getComponent<RenderComponent>();
                    if (playerRender) {
                        playerRender->getSprite().setColor(sf::Color(255, 100, 100));
                    }
                }
            }
        }
    );
}

// FIXED: All lambdas now have explicit return types and proper return statements
void registerGameEntities(b2World& world, TextureManager& textures) {
    EntityFactory& factory = EntityFactory::instance();

    // Register Player
    factory.registerCreator("Player", [&](float x, float y) -> std::unique_ptr<Entity> {
        return std::make_unique<PlayerEntity>(g_nextEntityId++, world, x, y, textures);
        });

    // Register Coin - FIXED: Always returns an entity
    factory.registerCreator("C", [&](float x, float y) -> std::unique_ptr<Entity> {
        auto entity = std::make_unique<CoinEntity>(g_nextEntityId++);

        // تعيين الموقع الأولي
        sf::Vector2f coinPosition(x + TILE_SIZE / 4.f, y + TILE_SIZE / 4.f);
        entity->addComponent<Transform>(coinPosition);

        auto* physics = entity->addComponent<PhysicsComponent>(world, b2_dynamicBody);
        physics->createCircleShape(15.0f); 
        physics->setPosition(coinPosition.x, coinPosition.y);

        if (auto* body = physics->getBody()) {
            body->SetGravityScale(0.0f); 
            body->SetLinearDamping(5.0f);
            body->SetFixedRotation(true); 
        }
        entity->setupCircularMotion(coinPosition);

        auto* render = entity->addComponent<RenderComponent>();
        if (render) {
            render->setTexture(textures.getResource("Coin.png"));
            auto& sprite = render->getSprite();
            sprite.setScale(0.08f, 0.08f);
            auto bounds = sprite.getLocalBounds();
            sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
            sprite.setPosition(coinPosition);
        }

        entity->addComponent<CollisionComponent>(CollisionComponent::CollisionType::Collectible);
        return entity;
        });
    

    // Register Gifts with level file characters - FIXED: Explicit return type
    auto registerGift = [&](const std::string& levelChar, GiftEntity::GiftType type) {
        factory.registerCreator(levelChar, [&, type](float x, float y) -> std::unique_ptr<Entity> {
            return std::make_unique<GiftEntity>(g_nextEntityId++, type, x, y, textures);
            });
        };

    registerGift("h", GiftEntity::GiftType::LifeHeart);
    registerGift("s", GiftEntity::GiftType::SpeedBoost);
    registerGift("p", GiftEntity::GiftType::Shield);
    registerGift("*", GiftEntity::GiftType::RareCoin);
    registerGift("r", GiftEntity::GiftType::ReverseMovement);
    registerGift("w", GiftEntity::GiftType::HeadwindStorm);
    registerGift("m", GiftEntity::GiftType::Magnetic);

    // Register Ground Tiles
    auto registerGround = [&](const std::string& levelChar, TileType type) {
        factory.registerCreator(levelChar, [&, type](float x, float y) -> std::unique_ptr<Entity> {
            return std::make_unique<GroundEntity>(g_nextEntityId++, type, world, x, y, textures);
            });
        };

    registerGround("G", TileType::Ground);
    registerGround("L", TileType::Left);
    registerGround("R", TileType::Right);
    registerGround("M", TileType::Middle);
    registerGround("E", TileType::Edge);

    // Register remaining entity types
    factory.registerCreator("S", [&](float x, float y) -> std::unique_ptr<Entity> {
        return std::make_unique<SeaEntity>(g_nextEntityId++, world, x, y, textures);
        });

    factory.registerCreator("X", [&](float x, float y) -> std::unique_ptr<Entity> {
        return std::make_unique<FlagEntity>(g_nextEntityId++, world, x, y, textures);
        });

    factory.registerCreator("c", [&](float x, float y) -> std::unique_ptr<Entity> {
        return std::make_unique<CactusEntity>(g_nextEntityId++, world, x, y, textures);
        });

    factory.registerCreator("B", [&](float x, float y) -> std::unique_ptr<Entity> {
        return std::make_unique<BoxEntity>(g_nextEntityId++, world, x, y, textures);
        });


    // Register Square Enemy
    factory.registerCreator("z", [&](float x, float y) -> std::unique_ptr<Entity> {
        std::cout << "[FACTORY] Creating SquareEnemyEntity at (" << x << ", " << y << ")" << std::endl;
        auto enemy = std::make_unique<SquareEnemyEntity>(g_nextEntityId++, world, x, y, textures);
        std::cout << "[FACTORY] SquareEnemyEntity created with ID: " << enemy->getId() << std::endl;
        return enemy;
        });

    // Register Smart Enemy (use 'Z' for smart enemy in level files)
    factory.registerCreator("Z", [&](float x, float y) -> std::unique_ptr<Entity> {
        std::cout << "[FACTORY] Creating SmartEnemyEntity at (" << x << ", " << y << ")" << std::endl;
        auto enemy = std::make_unique<SmartEnemyEntity>(g_nextEntityId++, world, x, y, textures);
        std::cout << "[FACTORY] SmartEnemyEntity created with ID: " << enemy->getId() << std::endl;
        return enemy;
        });

    // Register Falcon Enemy (use 'F' for falcon enemy in level files)
    factory.registerCreator("F", [&](float x, float y) -> std::unique_ptr<Entity> {
        std::cout << "[FACTORY] Creating FalconEnemyEntity at (" << x << ", " << y << ")" << std::endl;
        auto enemy = std::make_unique<FalconEnemyEntity>(g_nextEntityId++, world, x, y, textures);
        std::cout << "[FACTORY] FalconEnemyEntity created with ID: " << enemy->getId() << std::endl;
        return enemy;
        });
}