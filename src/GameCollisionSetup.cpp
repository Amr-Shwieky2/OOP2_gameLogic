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
#include <MagneticState.h>
#include <HeadwindState.h>
#include <ReversedState.h>
#include <Constants.h>
#include <SquareEnemyEntity.h>

// For entity ID generation
int g_nextEntityId = 1;

void setupGameCollisionHandlers(MultiMethodCollisionSystem& collisionSystem) {

    // Player vs Coin
    collisionSystem.registerHandler<PlayerEntity, CoinEntity>(
        [](PlayerEntity& player, CoinEntity& coin) {
            if (!coin.isActive()) return;

            player.addScore(10);
            coin.onCollect(&player);

            EventSystem::getInstance().publish(
                CoinCollectedEvent(player.getId(), 1)
            );

            std::cout << "Player collected coin! Score: " << player.getScore() << std::endl;
        }
    );

    // Player Projectile vs Smart Enemy - Specific handler
    collisionSystem.registerHandler<ProjectileEntity, SmartEnemyEntity>(
        [](ProjectileEntity& proj, SmartEnemyEntity& smartEnemy) {
            if (!proj.isFromPlayer() || !smartEnemy.isActive()) return;

            std::cout << "[PROJECTILE] Player projectile hit smart enemy!" << std::endl;

            auto* health = smartEnemy.getComponent<HealthComponent>();
            if (health) {
                health->takeDamage(1);

                if (!health->isAlive()) {
                    smartEnemy.setActive(false);
                    std::cout << "[PROJECTILE] Smart enemy killed by projectile! Health: 0" << std::endl;

                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(smartEnemy.getId(), proj.getId())
                    );
                }
                else {
                    std::cout << "[PROJECTILE] Smart enemy hit by projectile! Health: "
                        << health->getHealth() << std::endl;
                }
            }

            proj.setActive(false);
        }
    );


    // Player vs Regular Square Enemy
    collisionSystem.registerHandler<PlayerEntity, SquareEnemyEntity>(
        [](PlayerEntity& player, SquareEnemyEntity& enemy) {
            if (!enemy.isActive()) return;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* enemyHealth = enemy.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();
            auto* enemyPhysics = enemy.getComponent<PhysicsComponent>();

            if (!playerHealth || !enemyHealth || !playerPhysics || !enemyPhysics) return;

            sf::Vector2f playerPos = playerPhysics->getPosition();
            sf::Vector2f enemyPos = enemyPhysics->getPosition();

            if (playerPos.y < enemyPos.y - 20.0f) { // Player is above enemy
                std::cout << "[COLLISION] Player jumping on " << (int)enemy.getSizeType()
                    << " size square enemy!" << std::endl;

                // تحديد نوع الانقسام والنقاط
                int scoreBonus = 0;
                std::string splitMessage = "";

                switch (enemy.getSizeType()) {
                case SquareEnemyEntity::SizeType::Large:
                    scoreBonus = 150; // نقاط أكثر لأنه سينتج أعداء متوسطة
                    splitMessage = "Large enemy → 3 Medium enemies";
                    break;
                case SquareEnemyEntity::SizeType::Medium:
                    scoreBonus = 200; // نقاط أكثر لأنه سينتج أعداء أذكياء!
                    splitMessage = "Medium enemy → 3 SMART enemies! 🧠";
                    break;
                case SquareEnemyEntity::SizeType::Small:
                    scoreBonus = 100; // هذا لن يحدث عادة
                    splitMessage = "Small enemy defeated";
                    break;
                }

                std::cout << "[SCORE] " << splitMessage << " +" << scoreBonus << " points" << std::endl;

                // استدعاء onDeath قبل التعطيل لتفعيل الانقسام
                enemy.onDeath(&player);

                // قتل العدو الأصلي
                enemyHealth->takeDamage(999);
                enemy.setActive(false);

                // نطّ اللاعب
                playerPhysics->applyImpulse(0, -5.0f);

                // إضافة النقاط
                player.addScore(scoreBonus);

                // نشر الحدث
                EventSystem::getInstance().publish(
                    EnemyKilledEvent(enemy.getId(), player.getId())
                );
            }
            else {
                // العدو يؤذي اللاعب
                if (!playerHealth->isInvulnerable() && player.canTakeDamage()) {
                    playerHealth->takeDamage(1);
                    player.startDamageCooldown();

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
                player.applySpeedBoost(8.0f);
                std::cout << "Player collected Speed Boost!" << std::endl;
                break;

            case GiftEntity::GiftType::Shield:
                player.applyShield(7.0f);
                std::cout << "Player collected Shield!" << std::endl;
                break;

            case GiftEntity::GiftType::RareCoin:
                player.addScore(50);
                std::cout << "Player collected Rare Coin! +50 points" << std::endl;
                break;

            case GiftEntity::GiftType::ReverseMovement:
                player.changeState(ReversedState::getInstance());
                std::cout << "[WARNING] Player collected Reverse Movement! Controls inverted!" << std::endl;
                EventSystem::getInstance().publish(
                    PlayerStateChangedEvent("Normal", "Reversed")
                );
                break;

            case GiftEntity::GiftType::HeadwindStorm:
                player.changeState(HeadwindState::getInstance());
                std::cout << "[WARNING] Player collected Headwind Storm! Movement slowed!" << std::endl;
                EventSystem::getInstance().publish(
                    PlayerStateChangedEvent("Normal", "Headwind")
                );
                break;

            case GiftEntity::GiftType::Magnetic:
                player.changeState(MagneticState::getInstance());
                std::cout << "Player collected Magnetic! Coins will be attracted!" << std::endl;
                EventSystem::getInstance().publish(
                    PlayerStateChangedEvent("Normal", "Magnetic")
                );
                break;
            }

            gift.collect();
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

    // Player vs Cactus
    collisionSystem.registerHandler<PlayerEntity, CactusEntity>(
        [](PlayerEntity& player, CactusEntity& cactus) {
            auto* health = player.getComponent<HealthComponent>();
            if (health && !health->isInvulnerable() && player.canTakeDamage()) {
                health->takeDamage(1);
                player.startDamageCooldown();

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

    // Player vs Flag
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

    // ===== PROJECTILE HANDLERS =====

    // Projectile vs Regular Enemy (EXCLUDES Smart and Falcon enemies)
    collisionSystem.registerHandler<ProjectileEntity, EnemyEntity>(
        [](ProjectileEntity& proj, EnemyEntity& enemy) {
            if (!proj.isFromPlayer() || !enemy.isActive()) return;

            // IMPORTANT: Exclude specific enemy types that have their own handlers
            if (dynamic_cast<SmartEnemyEntity*>(&enemy)) {
                std::cout << "[DEBUG] Ignoring SmartEnemy in generic handler" << std::endl;
                return;
            }

            if (dynamic_cast<FalconEnemyEntity*>(&enemy)) {
                std::cout << "[DEBUG] Ignoring FalconEnemy in generic handler" << std::endl;
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

    // Enemy Projectile vs Player 
    collisionSystem.registerHandler<ProjectileEntity, PlayerEntity>(
        [](ProjectileEntity& proj, PlayerEntity& player) {
            if (proj.isFromPlayer() || !proj.isActive()) return;

            std::cout << "[Collision] Enemy projectile hit player!" << std::endl;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();

            if (playerHealth && !playerHealth->isInvulnerable() && player.canTakeDamage()) {
                playerHealth->takeDamage(1);
                player.startDamageCooldown();

                if (playerPhysics) {
                    playerPhysics->applyImpulse(0.5f, -1.5f);
                }

                auto* playerRender = player.getComponent<RenderComponent>();
                if (playerRender) {
                    playerRender->getSprite().setColor(sf::Color(255, 150, 150));
                }

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

    // Player vs Falcon direct contact 
    collisionSystem.registerHandler<PlayerEntity, FalconEnemyEntity>(
        [](PlayerEntity& player, FalconEnemyEntity& falcon) {
            if (!falcon.isActive()) return;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();
            auto* falconPhysics = falcon.getComponent<PhysicsComponent>();

            if (!playerHealth || !playerPhysics || !falconPhysics) return;

            std::cout << "[Collision] Player touched flying falcon!" << std::endl;

            sf::Vector2f playerPos = playerPhysics->getPosition();
            sf::Vector2f falconPos = falconPhysics->getPosition();

            if (playerPos.y < falconPos.y - 30.0f) {
                auto* falconHealth = falcon.getComponent<HealthComponent>();
                if (falconHealth) {
                    falconHealth->takeDamage(999);
                    falcon.setActive(false);

                    playerPhysics->applyImpulse(0, -4.0f);
                    player.addScore(200);

                    std::cout << "Player defeated flying falcon! +200 points" << std::endl;

                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(falcon.getId(), player.getId())
                    );
                }
            }
            else {
                if (!playerHealth->isInvulnerable() && player.canTakeDamage()) {
                    playerHealth->takeDamage(1);
                    player.startDamageCooldown();
                    std::cout << "Player hit by flying falcon! Health: "
                        << playerHealth->getHealth() << std::endl;

                    float knockbackDir = (playerPos.x > falconPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 4.0f, -3.0f);

                    auto* playerRender = player.getComponent<RenderComponent>();
                    if (playerRender) {
                        playerRender->getSprite().setColor(sf::Color(255, 100, 100));
                    }
                }
            }
        }
    );
    // Player vs smart enemy
    collisionSystem.registerHandler<PlayerEntity, SmartEnemyEntity>(
        [](PlayerEntity& player, SmartEnemyEntity& smartEnemy) {
            if (!smartEnemy.isActive()) return;

            auto* playerHealth = player.getComponent<HealthComponent>();
            auto* enemyHealth = smartEnemy.getComponent<HealthComponent>();
            auto* playerPhysics = player.getComponent<PhysicsComponent>();
            auto* enemyPhysics = smartEnemy.getComponent<PhysicsComponent>();

            if (!playerHealth || !enemyHealth || !playerPhysics || !enemyPhysics) return;

            // Get positions for collision detection
            sf::Vector2f playerPos = playerPhysics->getPosition();
            sf::Vector2f enemyPos = enemyPhysics->getPosition();

            float yDifference = playerPos.y - enemyPos.y;
            float xDifference = std::abs(playerPos.x - enemyPos.x);

            // Check if this is a jump attack (player is WELL ABOVE enemy)
            if (yDifference < -50.0f && xDifference < 40.0f) { 

                enemyHealth->takeDamage(1);

                if (!enemyHealth->isAlive()) {
                    smartEnemy.setActive(false);
                    player.addScore(250);

                    EventSystem::getInstance().publish(
                        EnemyKilledEvent(smartEnemy.getId(), player.getId())
                    );
                }
                else {
                    player.addScore(50);
                }

                // Bounce player
                playerPhysics->applyImpulse(0, -6.0f);
            }
            else {
                if (!playerHealth->isInvulnerable() && player.canTakeDamage()) {
                    playerHealth->takeDamage(1);
                    player.startDamageCooldown();
           
                    float knockbackDir = (playerPos.x > enemyPos.x) ? 1.0f : -1.0f;
                    playerPhysics->applyImpulse(knockbackDir * 5.0f, -3.0f);

                    auto* playerRender = player.getComponent<RenderComponent>();
                    if (playerRender) {
                        playerRender->getSprite().setColor(sf::Color(255, 100, 100));
                    }

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
            world, // You'll need access to the physics world here
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
}