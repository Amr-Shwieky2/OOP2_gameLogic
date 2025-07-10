// GameCollisionSetup.cpp - Updated for refactored PlayerEntity
#include "GameCollisionSetup.h"
#include "MultiMethodCollisionSystem.h"
#include "PlayerEntity.h"
#include "PlayerScoreManager.h"
#include "PlayerStateManager.h"
#include "PlayerVisualEffects.h"
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
#include <Constants.h>
#include <SquareEnemyEntity.h>
#include <WellEntity.h>

// For entity ID generation
int g_nextEntityId = 1;

void setupGameCollisionHandlers(MultiMethodCollisionSystem& collisionSystem) {

    // ===== Player vs Coin =====
    collisionSystem.registerHandler<PlayerEntity, CoinEntity>(
        [](PlayerEntity& player, CoinEntity& coin) {
            if (!coin.isActive()) return;

            // Use ScoreManager subsystem
            if (auto* scoreManager = player.getScoreManager()) {
                scoreManager->addScore(10);
                std::cout << "Player collected coin! Score: " << scoreManager->getScore() << std::endl;
            }

            coin.onCollect(&player);

            EventSystem::getInstance().publish(
                CoinCollectedEvent(player.getId(), 1)
            );
        }
    );

    // ===== Player vs Gift =====
    collisionSystem.registerHandler<PlayerEntity, GiftEntity>(
        [](PlayerEntity& player, GiftEntity& gift) {
            if (!gift.isActive() || gift.isCollected()) return;

            std::cout << "[Collision] Player collecting gift type: "
                << static_cast<int>(gift.getGiftType()) << std::endl;

            auto* stateManager = player.getStateManager();
            auto* scoreManager = player.getScoreManager();

            if (!stateManager || !scoreManager) {
                std::cerr << "[Collision] Player missing subsystems!" << std::endl;
                return;
            }

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
                stateManager->applySpeedBoost(5.0f);
                std::cout << "Player collected Speed Boost!" << std::endl;
                break;

            case GiftEntity::GiftType::Shield:
                stateManager->applyShield(6.0f);
                std::cout << "Player collected Shield!" << std::endl;
                break;

            case GiftEntity::GiftType::RareCoin:
                scoreManager->addScore(50);
                std::cout << "Player collected Rare Coin! +50 points" << std::endl;
                break;

            case GiftEntity::GiftType::ReverseMovement:
                stateManager->applyReverseEffect(10.0f);
                std::cout << "[WARNING] Player collected Reverse Movement! Controls inverted!" << std::endl;
                break;

            case GiftEntity::GiftType::HeadwindStorm:
                stateManager->applyHeadwindEffect(8.0f);
                std::cout << "[WARNING] Player collected Headwind Storm! Movement slowed!" << std::endl;
                break;

            case GiftEntity::GiftType::Magnetic:
                stateManager->applyMagneticEffect(6.0f);
                std::cout << "Player collected Magnetic! Coins will be attracted!" << std::endl;
                break;
            }

            gift.collect();
            EventSystem::getInstance().publish(
                ItemCollectedEvent(player.getId(), gift.getId(), ItemCollectedEvent::ItemType::Gift)
            );
        }
    );

    // ===== Player vs Sea =====
    collisionSystem.registerHandler<PlayerEntity, SeaEntity>(
        [](PlayerEntity& player, SeaEntity&) {
            auto* health = player.getComponent<HealthComponent>();
            auto* physics = player.getComponent<PhysicsComponent>();
            auto* visualEffects = player.getVisualEffects();

            if (health && !health->isInvulnerable()) {
                health->setHealth(0);

                // Use visual effects system for death effect
                if (visualEffects) {
                    auto* render = player.getComponent<RenderComponent>();
                    if (render) {
                        render->getSprite().setColor(sf::Color(100, 150, 255, 180));
                        render->getSprite().rotate(45.0f);
                    }
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

    // ===== Player vs Cactus =====
    collisionSystem.registerHandler<PlayerEntity, CactusEntity>(
        [](PlayerEntity& player, CactusEntity& cactus) {
            auto* health = player.getComponent<HealthComponent>();
            auto* visualEffects = player.getVisualEffects();

            if (health && !health->isInvulnerable() &&
                visualEffects && visualEffects->canTakeDamage()) {

                health->takeDamage(1);
                visualEffects->startDamageCooldown();

                auto* playerPhysics = player.getComponent<PhysicsComponent>();
                auto* cactusTransform = cactus.getComponent<Transform>();

                if (playerPhysics && cactusTransform) {
                    sf::Vector2f playerPos = playerPhysics->getPosition();
                    sf::Vector2f cactusPos = cactusTransform->getPosition();
                    float knockbackDir = (playerPos.x > cactusPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 4.0f, -2.0f);
                }

                // Use visual effects for damage indication
                if (visualEffects) {
                    visualEffects->startDamageEffect();
                }
            }
        }
    );

    // ===== Player vs Flag =====
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

            // Use score manager for flag bonus
            if (auto* scoreManager = player.getScoreManager()) {
                scoreManager->addScore(500);
            }

            std::cout << "Level Complete! Player reached the flag!" << std::endl;
        }
    );

    // ===== Player vs Square Enemy =====
    collisionSystem.registerHandler<PlayerEntity, SquareEnemyEntity>(
        [](PlayerEntity& player, SquareEnemyEntity& enemy) {
            if (!enemy.isActive()) return;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* enemyHealth = enemy.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();
            auto* enemyPhysics = enemy.getComponent<PhysicsComponent>();
            auto* visualEffects = player.getVisualEffects();
            auto* scoreManager = player.getScoreManager();

            if (!playerHealth || !enemyHealth || !playerPhysics || !enemyPhysics) return;

            sf::Vector2f playerPos = playerPhysics->getPosition();
            sf::Vector2f enemyPos = enemyPhysics->getPosition();

            // Check jump attack based on enemy size
            float sizeFactor = enemy.getSizeMultiplier();
            float verticalThreshold = TILE_SIZE * sizeFactor * 0.4f;

            if (playerPos.y < enemyPos.y - verticalThreshold) {
                std::cout << "[Collision] Player jumping on " << (int)enemy.getSizeType()
                    << " size square enemy!" << std::endl;

                int scoreBonus = (enemy.getSizeType() == SquareEnemyEntity::SizeType::Large) ? 150 : 100;

                enemy.onDeath(&player);
                enemyHealth->takeDamage(999);
                enemy.setActive(false);
                playerPhysics->applyImpulse(0, -5.0f);

                if (scoreManager) {
                    scoreManager->addScore(scoreBonus);
                }

                EventSystem::getInstance().publish(
                    EnemyKilledEvent(enemy.getId(), player.getId())
                );
            }
            else {
                // Player hit by enemy
                if (!playerHealth->isInvulnerable() &&
                    visualEffects && visualEffects->canTakeDamage()) {

                    playerHealth->takeDamage(1);
                    visualEffects->startDamageCooldown();

                    float knockbackDir = (playerPos.x > enemyPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 3.0f, -2.0f);

                    visualEffects->startDamageEffect();
                }
            }
        }
    );

    // ===== Player vs Smart Enemy =====
    collisionSystem.registerHandler<PlayerEntity, SmartEnemyEntity>(
        [](PlayerEntity& player, SmartEnemyEntity& smartEnemy) {
            if (!smartEnemy.isActive()) return;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* enemyHealth = smartEnemy.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();
            auto* enemyPhysics = smartEnemy.getComponent<PhysicsComponent>();
            auto* visualEffects = player.getVisualEffects();
            auto* scoreManager = player.getScoreManager();

            if (!playerHealth || !enemyHealth || !playerPhysics || !enemyPhysics) return;

            sf::Vector2f playerPos = playerPhysics->getPosition();
            sf::Vector2f enemyPos = enemyPhysics->getPosition();

            float yDifference = playerPos.y - enemyPos.y;
            float xDifference = std::abs(playerPos.x - enemyPos.x);

            // Check if this is a jump attack (player is WELL ABOVE enemy)
            if (yDifference < -50.0f && xDifference < 40.0f) {
                enemyHealth->takeDamage(1);

                if (!enemyHealth->isAlive()) {
                    smartEnemy.setActive(false);
                    if (scoreManager) {
                        scoreManager->addScore(250);
                    }

                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(smartEnemy.getId(), player.getId())
                    );
                }
                else {
                    if (scoreManager) {
                        scoreManager->addScore(50);
                    }
                }

                // Bounce player
                playerPhysics->applyImpulse(0, -4.0f);
            }
            else {
                // Player hit by smart enemy
                if (!playerHealth->isInvulnerable() &&
                    visualEffects && visualEffects->canTakeDamage()) {

                    playerHealth->takeDamage(1);
                    visualEffects->startDamageCooldown();

                    float knockbackDir = (playerPos.x > enemyPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 5.0f, -3.0f);

                    visualEffects->startDamageEffect();

                    if (!playerHealth->isAlive()) {
                        EventSystem::getInstance().publish(
                            PlayerDiedEvent(player.getId())
                        );
                    }

                    // Visual feedback for smart enemy
                    auto* enemyRender = smartEnemy.getComponent<RenderComponent>();
                    if (enemyRender) {
                        enemyRender->getSprite().setColor(sf::Color(255, 200, 100));
                    }
                }
            }
        }
    );

    // ===== Player vs Falcon Enemy =====
    collisionSystem.registerHandler<PlayerEntity, FalconEnemyEntity>(
        [](PlayerEntity& player, FalconEnemyEntity& falcon) {
            if (!falcon.isActive()) return;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();
            auto* falconPhysics = falcon.getComponent<PhysicsComponent>();
            auto* visualEffects = player.getVisualEffects();
            auto* scoreManager = player.getScoreManager();

            if (!playerHealth || !playerPhysics || !falconPhysics) return;

            std::cout << "[Collision] Player touched flying falcon!" << std::endl;

            sf::Vector2f playerPos = playerPhysics->getPosition();
            sf::Vector2f falconPos = falconPhysics->getPosition();

            if (playerPos.y < falconPos.y - 30.0f) {
                // Player jumped on falcon
                auto* falconHealth = falcon.getComponent<HealthComponent>();
                if (falconHealth) {
                    falconHealth->takeDamage(999);
                    falcon.setActive(false);

                    playerPhysics->applyImpulse(0, -4.0f);
                    if (scoreManager) {
                        scoreManager->addScore(200);
                    }

                    std::cout << "Player defeated flying falcon! +200 points" << std::endl;

                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(falcon.getId(), player.getId())
                    );
                }
            }
            else {
                // Player hit by falcon
                if (!playerHealth->isInvulnerable() &&
                    visualEffects && visualEffects->canTakeDamage()) {

                    playerHealth->takeDamage(1);
                    visualEffects->startDamageCooldown();

                    std::cout << "Player hit by flying falcon! Health: "
                        << playerHealth->getHealth() << std::endl;

                    float knockbackDir = (playerPos.x > falconPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 4.0f, -3.0f);

                    visualEffects->startDamageEffect();
                }
            }
        }
    );

    // ===== Projectile vs Smart Enemy =====
    collisionSystem.registerHandler<ProjectileEntity, SmartEnemyEntity>(
        [](ProjectileEntity& proj, SmartEnemyEntity& smartEnemy) {
            if (!proj.isFromPlayer() || !smartEnemy.isActive()) return;

            std::cout << "[PROJECTILE] Player projectile hit smart enemy!" << std::endl;

            auto* health = smartEnemy.getComponent<HealthComponent>();
            if (health) {
                health->takeDamage(1);

                if (!health->isAlive()) {
                    smartEnemy.setActive(false);
                    std::cout << "[PROJECTILE] Smart enemy killed by projectile!" << std::endl;

                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(smartEnemy.getId(), proj.getId())
                    );
                }
                else {
                    std::cout << "[PROJECTILE] Smart enemy hit! Health: "
                        << health->getHealth() << std::endl;
                }
            }

            proj.setActive(false);
        }
    );

    // ===== Projectile vs Regular Enemy =====
    collisionSystem.registerHandler<ProjectileEntity, EnemyEntity>(
        [](ProjectileEntity& proj, EnemyEntity& enemy) {
            if (!proj.isFromPlayer() || !enemy.isActive()) return;

            // Exclude specific enemy types that have their own handlers
            if (dynamic_cast<SmartEnemyEntity*>(&enemy)) {
                return;
            }

            if (dynamic_cast<FalconEnemyEntity*>(&enemy)) {
                return;
            }

            std::cout << "[PROJECTILE] Projectile hit regular enemy" << std::endl;

            auto* health = enemy.getComponent<HealthComponent>();
            if (health) {
                health->takeDamage(1);
                if (!health->isAlive()) {
                    enemy.setActive(false);
                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(enemy.getId(), proj.getId())
                    );
                }
            }

            proj.setActive(false);
        }
    );

    // ===== Projectile vs Falcon Enemy =====
    collisionSystem.registerHandler<ProjectileEntity, FalconEnemyEntity>(
        [](ProjectileEntity& proj, FalconEnemyEntity& falcon) {
            if (!proj.isFromPlayer() || !falcon.isActive()) return;

            std::cout << "[PROJECTILE] Player projectile hit falcon enemy!" << std::endl;

            auto* health = falcon.getComponent<HealthComponent>();
            if (health) {
                health->takeDamage(1);
                if (!health->isAlive()) {
                    falcon.setActive(false);
                    std::cout << "[PROJECTILE] Falcon enemy killed by projectile!" << std::endl;

                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(falcon.getId(), proj.getId())
                    );
                }
                else {
                    std::cout << "[PROJECTILE] Falcon enemy hit! Health: "
                        << health->getHealth() << std::endl;
                }
            }

            proj.setActive(false);
        }
    );

    // ===== Projectile vs Ground =====
    collisionSystem.registerHandler<ProjectileEntity, GroundEntity>(
        [](ProjectileEntity& proj, GroundEntity&) {
            // Projectile hits ground - destroy it
            std::cout << "[PROJECTILE] Projectile hit ground!" << std::endl;
            
            // Create a simple visual effect (could be expanded)
            auto* render = proj.getComponent<RenderComponent>();
            if (render) {
                render->getSprite().setColor(sf::Color(200, 200, 200, 150)); // Fade out
                render->getSprite().setScale(0.05f, 0.05f); // Smaller
            }
            
            // Deactivate the projectile
            proj.setActive(false);
        }
    );

    // ===== Enemy Projectile vs Player =====
    collisionSystem.registerHandler<ProjectileEntity, PlayerEntity>(
        [](ProjectileEntity& proj, PlayerEntity& player) {
            if (proj.isFromPlayer() || !proj.isActive()) return;

            std::cout << "[Collision] Enemy projectile hit player!" << std::endl;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();
            auto* visualEffects = player.getVisualEffects();

            if (playerHealth && !playerHealth->isInvulnerable() &&
                visualEffects && visualEffects->canTakeDamage()) {

                playerHealth->takeDamage(1);
                visualEffects->startDamageCooldown();

                if (playerPhysics) {
                    playerPhysics->applyImpulse(0.5f, -1.5f);
                }

                visualEffects->startDamageEffect();

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

            proj.setActive(false);
        }
    );
    // ===== Player vs Well =====
    collisionSystem.registerHandler<PlayerEntity, WellEntity>(
        [](PlayerEntity& player, WellEntity& well) {
            try {
                if (!player.isActive() || !well.isActive() || well.isActivated()) {
                    return;
                }

                std::cout << "[COLLISION] Player entered well - processing safely..." << std::endl;

                // تفعيل البئر (يطلب تغيير المستوى)
                well.onPlayerEnter();

                // إضافة النقاط
                if (auto* scoreManager = player.getScoreManager()) {
                    scoreManager->addScore(100);
                }

                std::cout << "[COLLISION] Well activated - level change requested" << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "[COLLISION] Exception: " << e.what() << std::endl;
            }
        }
    );
}

void registerGameEntities(b2World& world, TextureManager& textures) {
    EntityFactory& factory = EntityFactory::instance();

    // Register Player
    factory.registerCreator("Player", [&](float x, float y) -> std::unique_ptr<Entity> {
        return std::make_unique<PlayerEntity>(g_nextEntityId++, world, x, y, textures);
        });

    // Register Coin
    factory.registerCreator("C", [&](float x, float y) -> std::unique_ptr<Entity> {
        auto entity = std::make_unique<CoinEntity>(g_nextEntityId++);

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

    // Register Gifts
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
        std::cout << "[FACTORY] Creating Large SquareEnemyEntity at (" << x << ", " << y << ")" << std::endl;
        auto enemy = std::make_unique<SquareEnemyEntity>(
            g_nextEntityId++,
            world,
            x, y,
            textures,
            SquareEnemyEntity::SizeType::Large
        );
        std::cout << "[FACTORY] SquareEnemyEntity created with ID: " << enemy->getId() << std::endl;
        return enemy;
        }
    );

    // Register Smart Enemy
    factory.registerCreator("Z", [&](float x, float y) -> std::unique_ptr<Entity> {
        std::cout << "[FACTORY] Creating SmartEnemyEntity at (" << x << ", " << y << ")" << std::endl;
        auto enemy = std::make_unique<SmartEnemyEntity>(g_nextEntityId++, world, x, y, textures);
        std::cout << "[FACTORY] SmartEnemyEntity created with ID: " << enemy->getId() << std::endl;
        return enemy;
        });

    // Register Falcon Enemy
    factory.registerCreator("F", [&](float x, float y) -> std::unique_ptr<Entity> {
        std::cout << "[FACTORY] Creating FalconEnemyEntity at (" << x << ", " << y << ")" << std::endl;
        auto enemy = std::make_unique<FalconEnemyEntity>(g_nextEntityId++, world, x, y, textures);
        std::cout << "[FACTORY] FalconEnemyEntity created with ID: " << enemy->getId() << std::endl;
        return enemy;
        });
    factory.registerCreator("W", [&](float x, float y) -> std::unique_ptr<Entity> {
        std::cout << "[FACTORY] Creating Well at (" << x << ", " << y << ")" << std::endl;
        return std::make_unique<WellEntity>(g_nextEntityId++, world, x, y, textures);
        });
}