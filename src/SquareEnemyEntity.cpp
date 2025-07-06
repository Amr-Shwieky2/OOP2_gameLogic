#include "SquareEnemyEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "AIComponent.h"
#include "PatrolStrategy.h"
#include "GameSession.h"
#include "Constants.h"
#include <iostream>
#include <random>
#include <MovementComponent.h>
#include <SmartEnemyEntity.h>

extern int g_nextEntityId;
extern GameSession* g_currentSession;

SquareEnemyEntity::SquareEnemyEntity(IdType id, b2World& world, float x, float y,
    TextureManager& textures, SizeType size)
    : EnemyEntity(id, EnemyType::Square, world, x, y, textures)
    , m_sizeType(size) {
    std::cout << "[SQUARE ENEMY] Creating size " << (int)size << " enemy" << std::endl;
    setupComponents(world, x, y, textures);
}

void SquareEnemyEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    // Position enemy at tile center
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f;

    // Add Transform
    auto* transform = addComponent<Transform>(sf::Vector2f(centerX, centerY));
    transform->setPosition(centerX, centerY);

    // Get size-based parameters
    float sizeMultiplier = getSizeMultiplier();
    float physicsSize = TILE_SIZE * sizeMultiplier;

    // Add PhysicsComponent
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createBoxShape(physicsSize, physicsSize,
        1.0f,    // density
        0.3f,    // friction
        0.0f);   // restitution
    physics->setPosition(centerX, centerY);

    if (auto* body = physics->getBody()) {
        body->SetFixedRotation(true);
        body->SetGravityScale(1.0f);
        body->SetLinearDamping(0.0f);  // مهم لعدم كبح الحركة
        body->SetAwake(true);
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    // Add RenderComponent
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("SquareEnemy2.png"));
    auto& sprite = render->getSprite();

    float renderScale = sizeMultiplier * 0.3f;
    sprite.setScale(renderScale, renderScale);

    sf::Color enemyColor;
    switch (m_sizeType) {
    case SizeType::Large:  enemyColor = sf::Color(255, 100, 100); break;
    case SizeType::Medium: enemyColor = sf::Color(255, 150, 100); break;
    case SizeType::Small:  enemyColor = sf::Color(255, 200, 100); break;
    }
    sprite.setColor(enemyColor);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite.setPosition(centerX, centerY);

    // ✅ Add HealthComponent (مهم جدًا!)
    addComponent<HealthComponent>(static_cast<int>(getHealthForSize()));

    // ✅ Add CollisionComponent
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Enemy);

    // ✅ Add MovementComponent (مطلوب لـ AI)
    addComponent<MovementComponent>();

    // ✅ Add AIComponent (مثل PatrolStrategy)
    float patrolDistance = 200.0f * sizeMultiplier;
    float speed = getSpeedForSize();
    addComponent<AIComponent>(std::make_unique<PatrolStrategy>(patrolDistance, speed));

    std::cout << "[SQUARE ENEMY] Size " << (int)m_sizeType
        << " setup complete - Scale: " << renderScale
        << " Health: " << getHealthForSize()
        << " Speed: " << speed << std::endl;
}


void SquareEnemyEntity::onDeath(Entity* killer) {
    if (killer && canSplit()) {
        auto* transform = getComponent<Transform>();
        if (transform) {
            sf::Vector2f deathPos = transform->getPosition();

            if (m_sizeType == SizeType::Large) {
                std::cout << "[SQUARE ENEMY] LARGE enemy killed! Splitting into 3 MEDIUM enemies!" << std::endl;
            }
            else if (m_sizeType == SizeType::Medium) {
                std::cout << "[SQUARE ENEMY] MEDIUM enemy killed! Spawning 3 SMART ENEMIES! 🧠⚡" << std::endl;
            }

            spawnSplitEnemies(deathPos);
        }
    }

    // Call base death handling
    EnemyEntity::onDeath(killer);
}


void SquareEnemyEntity::spawnSplitEnemies(const sf::Vector2f& deathPosition) {
    if (!g_currentSession) {
        std::cerr << "[SQUARE ENEMY] No game session for spawning split enemies" << std::endl;
        return;
    }

    // Get physics world from our own physics component
    auto* physics = getComponent<PhysicsComponent>();
    if (!physics || !physics->getBody()) {
        std::cerr << "[SQUARE ENEMY] No physics component for world access" << std::endl;
        return;
    }

    b2World& world = *physics->getBody()->GetWorld();

    // Random number generator for spawn positions
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> distanceDist(50.0f, 100.0f);

    // تحديد نوع العدو حسب الحجم الحالي
    if (m_sizeType == SizeType::Large) {
        // الحجم الكبير ينقسم إلى 3 أعداء متوسطة عادية
        std::cout << "[SQUARE ENEMY] Large enemy splitting into 3 Medium SquareEnemies" << std::endl;

        for (int i = 0; i < 3; ++i) {
            float angle = (2.0f * 3.14159f * i / 3.0f) + angleDist(gen) * 0.3f;
            float distance = distanceDist(gen);

            sf::Vector2f spawnOffset(
                std::cos(angle) * distance,
                std::sin(angle) * distance
            );

            sf::Vector2f spawnPos = deathPosition + spawnOffset;
            spawnPos.y = std::max(spawnPos.y, 100.0f);

            try {
                // إنشاء عدو مربع متوسط
                auto mediumEnemy = std::make_unique<SquareEnemyEntity>(
                    g_nextEntityId++,
                    world,
                    spawnPos.x - TILE_SIZE / 2.f,
                    spawnPos.y - TILE_SIZE / 2.f,
                    getTextures(),
                    SizeType::Medium
                );

                // إضافة سرعة أولية
                auto* mediumPhysics = mediumEnemy->getComponent<PhysicsComponent>();
                if (mediumPhysics) {
                    sf::Vector2f velocity(
                        std::cos(angle) * 2.0f,  // انتشار أفقي
                        -3.0f                    // سرعة عمودية للأعلى
                    );
                    mediumPhysics->setVelocity(velocity.x, velocity.y);
                }

                g_currentSession->spawnEntity(std::move(mediumEnemy));
                std::cout << "[SQUARE ENEMY] Spawned medium enemy " << (i + 1) << "/3" << std::endl;

            }
            catch (const std::exception& e) {
                std::cerr << "[SQUARE ENEMY] Error spawning medium enemy: " << e.what() << std::endl;
            }
        }
    }
    else if (m_sizeType == SizeType::Medium) {
        // الحجم المتوسط ينقسم إلى 3 أعداء أذكياء!
        std::cout << "[SQUARE ENEMY] Medium enemy splitting into 3 Smart Enemies!" << std::endl;

        for (int i = 0; i < 3; ++i) {
            float angle = (2.0f * 3.14159f * i / 3.0f) + angleDist(gen) * 0.3f;
            float distance = distanceDist(gen);

            sf::Vector2f spawnOffset(
                std::cos(angle) * distance,
                std::sin(angle) * distance
            );

            sf::Vector2f spawnPos = deathPosition + spawnOffset;
            spawnPos.y = std::max(spawnPos.y, 100.0f);

            try {
                // إنشاء عدو ذكي بدلاً من عدو مربع صغير
                auto smartEnemy = std::make_unique<SmartEnemyEntity>(
                    g_nextEntityId++,
                    world,
                    spawnPos.x - TILE_SIZE / 2.f,
                    spawnPos.y - TILE_SIZE / 2.f,
                    getTextures()
                );

                // إضافة سرعة أولية مع تأثير درامي أكبر للأعداء الأذكياء
                auto* smartPhysics = smartEnemy->getComponent<PhysicsComponent>();
                if (smartPhysics) {
                    sf::Vector2f velocity(
                        std::cos(angle) * 3.0f,  // سرعة أفقية أكبر للأعداء الأذكياء
                        -4.0f                    // قفزة أعلى للأعداء الأذكياء
                    );
                    smartPhysics->setVelocity(velocity.x, velocity.y);
                }

                g_currentSession->spawnEntity(std::move(smartEnemy));
                std::cout << "[SQUARE ENEMY] Spawned SMART enemy " << (i + 1)
                    << "/3 at (" << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;

            }
            catch (const std::exception& e) {
                std::cerr << "[SQUARE ENEMY] Error spawning smart enemy: " << e.what() << std::endl;
            }
        }

        std::cout << "[SQUARE ENEMY] Successfully spawned 3 SMART ENEMIES! Game difficulty increased!" << std::endl;
    }
    // الحجم الصغير لا ينقسم (هذا الكود لن يُستدعى عادة لأن Small enemies لا تنقسم)
}

float SquareEnemyEntity::getSizeMultiplier() const {
    switch (m_sizeType) {
    case SizeType::Large:  return LARGE_SIZE;
    case SizeType::Medium: return MEDIUM_SIZE;
    case SizeType::Small:  return SMALL_SIZE;
    default:               return LARGE_SIZE;
    }
}

float SquareEnemyEntity::getHealthForSize() const {
    switch (m_sizeType) {
    case SizeType::Large:  return 3.0f;  // Takes 3 hits
    case SizeType::Medium: return 2.0f;  // Takes 2 hits  
    case SizeType::Small:  return 1.0f;  // Takes 1 hit
    default:               return 1.0f;
    }
}

float SquareEnemyEntity::getSpeedForSize() const {
    switch (m_sizeType) {
    case SizeType::Large:  return 60.0f;  // Slowest
    case SizeType::Medium: return 80.0f;  // Medium speed
    case SizeType::Small:  return 120.0f; // Fastest
    default:               return 80.0f;
    }
}