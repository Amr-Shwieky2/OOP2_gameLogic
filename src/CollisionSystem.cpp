#include "CollisionSystem.h"
#include <iostream>
#include <SquareEnemy.h>
#include <Sea.h>
#include <MagneticGift.h>
#include <Cactus.h>
#include <FalconEnemy.h>

CollisionSystem::CollisionSystem(Player& player, std::function<void(std::unique_ptr<GameObject>)> spawnCallback)
    : m_player(player), m_spawnCallback(spawnCallback) {
    setupCollisionHandlers();
}

void CollisionSystem::setupCollisionHandlers() {
    // ======= Player Collisions =======
    m_collisionHandler.registerHandler<Player, Coin>(
        [](Player& player, Coin& coin) {
            if (!coin.isCollected()) {
                coin.collect();
                player.increaseScore(10);
            }
        }
    );

    m_collisionHandler.registerHandler<Player, MovableBox>(
        [](Player& player, MovableBox& box) {
            sf::FloatRect playerBounds = player.getBounds();
            sf::FloatRect boxBounds = box.getBounds();

            float playerCenterX = playerBounds.left + playerBounds.width / 2.0f;
            float boxCenterX = boxBounds.left + boxBounds.width / 2.0f;

            float forceDirection = (playerCenterX < boxCenterX) ? 1.0f : -1.0f;
            float pushForce = 150.0f;

            sf::Vector2f playerVelocity = player.getVelocity();
            float velocityMultiplier = std::abs(playerVelocity.x) / 100.0f;
            pushForce *= (1.0f + velocityMultiplier);

            box.applyForce(forceDirection * pushForce, 0.0f);
        }
    );

    m_collisionHandler.registerHandler<Player, GroundTile>(
        [](Player& player, GroundTile&) {
            player.beginContact();
        }
    );
    m_collisionHandler.registerHandler<GroundTile, Player>(
        [](GroundTile&, Player& player) {
            player.endContact();
        }
    );

    m_collisionHandler.registerHandler<Player, SquareEnemy>(
        [](Player& player, SquareEnemy& enemy) {
            if (!enemy.isAlive()) return;

            auto playerBounds = player.getBounds();
            auto enemyBounds = enemy.getBounds();

            float playerBottom = playerBounds.top + playerBounds.height;
            float enemyTop = enemyBounds.top;

            if (playerBottom < enemyTop + 10.f) {
                enemy.kill();
                player.applyJumpImpulse();
            }
            else if (!player.hasEffect(PlayerEffect::Shield) && enemy.canDamage()) {
                player.loseLife();
                enemy.startDamageCooldown();
            }
        }
    );

    m_collisionHandler.registerHandler<Player, Cactus>(
        [](Player& player, Cactus& cactus) {
            if (player.canTakeCactusDamage() && !player.hasEffect(PlayerEffect::Shield)) {
                player.loseLife();
                player.resetCactusCooldown();
            }
            player.pushBackFrom(cactus.getBounds().getPosition());
        }
    );

    m_collisionHandler.registerHandler<Player, Projectile>(
        [](Player& player, Projectile& proj) {
            if (proj.isAlive() && proj.isEnemyShot()) {
                player.loseLife();
                proj.destroy();
            }
        }
    );

    // ======= Projectile Collisions =======
    m_collisionHandler.registerHandler<SquareEnemy, Projectile>(
        [](SquareEnemy& enemy, Projectile& proj) {
            if (proj.isAlive() && !proj.isEnemyShot() && enemy.isAlive()) {
                enemy.kill();
                proj.destroy();
            }
        }
    );

    m_collisionHandler.registerHandler<FalconEnemy, Projectile>(
        [](FalconEnemy& enemy, Projectile& proj) {
            if (proj.isAlive() && !proj.isEnemyShot()) {
                enemy.kill();
                proj.destroy();
            }
        }
    );

    m_collisionHandler.registerHandler<GroundTile, Projectile>(
        [](GroundTile&, Projectile& proj) {
            if (proj.isAlive()) {
                proj.destroy();
            }
        }
    );

    m_collisionHandler.registerHandler<Cactus, Projectile>(
        [](Cactus&, Projectile& proj) {
            if (proj.isAlive()) {
                proj.destroy();
            }
        }
    );

    // ======= Enemy Collisions =======
    m_collisionHandler.registerHandler<SquareEnemy, Cactus>(
        [](SquareEnemy& enemy, Cactus& cactus) {
            enemy.blockMovement(); // You must implement this in SquareEnemy
        }
    );

    // ======= Sea =======
    m_collisionHandler.registerHandler<Player, Sea>(
        [](Player& player, Sea& sea) {
            sea.onPlayerContact(player);
        }
    );

    // ======= Gifts =======
    m_collisionHandler.registerHandler<Player, LifeHeartGift>(
        [](Player& player, LifeHeartGift& gift) {
            if (!gift.isCollected()) gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, SpeedGift>(
        [](Player& player, SpeedGift& gift) {
            if (!gift.isCollected()) gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, ReverseMovementGift>(
        [](Player& player, ReverseMovementGift& gift) {
            if (!gift.isCollected()) gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, RareCoinGift>(
        [](Player& player, RareCoinGift& gift) {
            if (!gift.isCollected()) gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, ProtectiveShieldGift>(
        [](Player& player, ProtectiveShieldGift& gift) {
            if (!gift.isCollected()) gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, HeadwindStormGift>(
        [](Player& player, HeadwindStormGift& gift) {
            if (!gift.isCollected()) gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, MagneticGift>(
        [](Player& player, MagneticGift& gift) {
            if (!gift.isCollected()) gift.onCollect(player);
        });
}

void CollisionSystem::checkCollisions(std::vector<std::unique_ptr<GameObject>>& objects) {
    for (auto& obj : objects) {
        if (areColliding(m_player, *obj)) {
            m_collisionHandler.handleCollision(m_player, *obj);
        }

        for (auto& other : objects) {
            if (obj != other && areColliding(*obj, *other)) {
                m_collisionHandler.handleCollision(*obj, *other);
            }
        }
    }
}

bool CollisionSystem::areColliding(const GameObject& obj1, const GameObject& obj2) const {
    sf::FloatRect bounds1 = obj1.getBounds();
    sf::FloatRect bounds2 = obj2.getBounds();
    return bounds1.intersects(bounds2);
}
