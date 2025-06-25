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
            // حساب اتجاه الدفع بناءً على موقع اللاعب والصندوق
            sf::FloatRect playerBounds = player.getBounds();
            sf::FloatRect boxBounds = box.getBounds();

            float playerCenterX = playerBounds.left + playerBounds.width / 2.0f;
            float boxCenterX = boxBounds.left + boxBounds.width / 2.0f;
            // تحديد اتجاه الدفع
            float forceDirection = (playerCenterX < boxCenterX) ? 1.0f : -1.0f;
            // قوة الدفع (يمكن تعديلها حسب سرعة اللاعب)
            float pushForce = 150.0f; // قوة أساسية
            // إذا كان اللاعب يتحرك، زيد القوة
            sf::Vector2f playerVelocity = player.getVelocity(); // بحاجة لهذه الدالة في Player
            float velocityMultiplier = std::abs(playerVelocity.x) / 100.0f; // تطبيع السرعة
            pushForce *= (1.0f + velocityMultiplier);
            // تطبيق القوة على الصندوق
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
    m_collisionHandler.registerHandler<GroundTile, Projectile>(
        [](GroundTile&, Projectile& proj) {
            if (proj.isAlive() ) {
                proj.destroy();
            }
        });


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
            else {
                // Side hit logic with cooldown
                if (!player.hasEffect(PlayerEffect::Shield) && enemy.canDamage()) {
                    player.loseLife();
                    enemy.startDamageCooldown();
                }
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
            if (proj.isEnemyShot() && proj.isAlive()) {
                player.loseLife();
                proj.destroy();
            }
        }
    );
	m_collisionHandler.registerHandler<SquareEnemy, Projectile>(
        [](SquareEnemy& enemy, Projectile& proj) {
		if (proj.isAlive() && !proj.isEnemyShot()) {
			enemy.kill();
			proj.destroy();
		}
		});

    m_collisionHandler.registerHandler<FalconEnemy, Projectile>(
        [](FalconEnemy& enemy, Projectile& proj) {
            if (proj.isAlive() && !proj.isEnemyShot()) {
                enemy.kill();
                proj.destroy();
            }
        });


    m_collisionHandler.registerHandler<Player, Sea>(
        [](Player& player, Sea& sea) {
            sea.onPlayerContact(player);
        }
    );

    m_collisionHandler.registerHandler<Player, LifeHeartGift>(
        [](Player& player, LifeHeartGift& gift) {
            if (!gift.isCollected())
                gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, SpeedGift>(
        [](Player& player, SpeedGift& gift) {
            if (!gift.isCollected())
                gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, ReverseMovementGift>(
        [](Player& player, ReverseMovementGift& gift) {
            if (!gift.isCollected())
                gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, RareCoinGift>(
        [](Player& player, RareCoinGift& gift) {
            if (!gift.isCollected())
                gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, ProtectiveShieldGift>(
        [](Player& player, ProtectiveShieldGift& gift) {
            if (!gift.isCollected())
                gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, HeadwindStormGift>(
        [](Player& player, HeadwindStormGift& gift) {
            if (!gift.isCollected())
                gift.onCollect(player);
        });

    m_collisionHandler.registerHandler<Player, MagneticGift>(
        [](Player& player, MagneticGift& gift) {
            if (!gift.isCollected())
                gift.onCollect(player);
        });
}

void CollisionSystem::checkCollisions(std::vector<std::unique_ptr<GameObject>>& objects) {
    for (auto& obj : objects) {
        if (areColliding(m_player, *obj)) {
            // استخدام multimethods لمعالجة التصادم
            bool handled = m_collisionHandler.handleCollision(m_player, *obj);
        }
    }
}

bool CollisionSystem::areColliding(const GameObject& obj1, const GameObject& obj2) const {
    sf::FloatRect bounds1 = obj1.getBounds();
    sf::FloatRect bounds2 = obj2.getBounds();
    return bounds1.intersects(bounds2);
}