#include "GameCollisionSetup.h"
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

// For entity ID generation
// This needs external linkage so other modules (like SurpriseBoxManager)
// can reference it. Previously it was declared static which limited its
// visibility and caused linker errors.
int g_nextEntityId = 1;

void setupGameCollisionHandlers(MultiMethodCollisionSystem& collisionSystem) {

    // Player vs Coin
    collisionSystem.registerHandler<PlayerEntity, CoinEntity>(
        [](PlayerEntity& player, CoinEntity& coin) {
            if (!coin.isActive()) return;

            player.addScore(10);
            coin.onCollect(&player);

            // Publish item collected event
            EventSystem::getInstance().publish(
                ItemCollectedEvent(player.getId(), coin.getId(), ItemCollectedEvent::ItemType::Coin)
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

            switch (gift.getGiftType()) {
            case GiftEntity::GiftType::LifeHeart: {
                auto* health = player.getComponent<HealthComponent>();
                if (health) health->heal(1);
                std::cout << "Player collected Life Heart!" << std::endl;
                break;
            }

            case GiftEntity::GiftType::SpeedBoost:
                player.applySpeedBoost(5.0f);
                std::cout << "Player collected Speed Boost!" << std::endl;
                break;

            case GiftEntity::GiftType::Shield:
                player.applyShield(7.0f);
                std::cout << "Player collected Shield!" << std::endl;
                break;

            case GiftEntity::GiftType::RareCoin:
                player.addScore(50);
                std::cout << "Player collected Rare Coin!" << std::endl;
                break;

                // TODO: Implement other gift effects
            default:
                break;
            }

            gift.collect();
        }
    );

    // Player vs Sea (instant death)
    collisionSystem.registerHandler<PlayerEntity, SeaEntity>(
        [](PlayerEntity& player, SeaEntity& sea) {
            auto* health = player.getComponent<HealthComponent>();
            if (health && !health->isInvulnerable()) {
                health->setHealth(0); // Instant death
                std::cout << "Player fell in the sea!" << std::endl;
            }
        }
    );

    // Player vs Cactus (damage)
    collisionSystem.registerHandler<PlayerEntity, CactusEntity>(
        [](PlayerEntity& player, CactusEntity& cactus) {
            auto* health = player.getComponent<HealthComponent>();
            if (health && !health->isInvulnerable()) {
                health->takeDamage(1);

                // Knockback
                auto* playerPhysics = player.getComponent<PhysicsComponent>();
                auto* cactusTransform = cactus.getComponent<Transform>();
                if (playerPhysics && cactusTransform) {
                    sf::Vector2f playerPos = playerPhysics->getPosition();
                    sf::Vector2f cactusPos = cactusTransform->getPosition();

                    float knockbackDir = (playerPos.x > cactusPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 2.0f, -1.0f);
                }

                std::cout << "Player hit cactus! Health: " << health->getHealth() << std::endl;
            }
        }
    );

    // Player vs Flag (level complete)
    collisionSystem.registerHandler<PlayerEntity, FlagEntity>(
        [](PlayerEntity& player, FlagEntity& flag) {
            std::cout << "Level Complete! Player reached the flag!" << std::endl;
            // TODO: Trigger level complete event
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
        entity->addComponent<Transform>(sf::Vector2f(x, y));

        auto* render = entity->addComponent<RenderComponent>();
        if (render) {
            render->setTexture(textures.getResource("Coin.png"));
            auto& sprite = render->getSprite();
            sprite.setScale(0.08f, 0.08f);
            auto bounds = sprite.getLocalBounds();
            sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        }

        entity->addComponent<CollisionComponent>(CollisionComponent::CollisionType::Collectible);
        return entity;  // Always return, regardless of render component
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