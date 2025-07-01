// SurpriseBoxManager.cpp
#include "SurpriseBoxManager.h"
#include "EntityManager.h"
#include "EntityFactory.h"
#include "PlayerEntity.h"
#include "GiftEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include <iostream>

// For entity ID generation
extern int g_nextEntityId;

SurpriseBoxManager::SurpriseBoxManager(TextureManager& textures, sf::RenderWindow& window)
    : m_textures(textures)
    , m_window(&window)
    , m_gen(std::random_device{}())
{
    // Create surprise box screen
    m_surpriseScreen = std::make_unique<SurpriseBoxScreen>(window, textures);

    // Subscribe to coin collected events
    EventSystem::getInstance().subscribe<CoinCollectedEvent>(
        [this](const CoinCollectedEvent& event) {
            this->onCoinCollected();
        }
    );
}

bool SurpriseBoxManager::shouldTriggerSurprise() const {
    // Changed to 5 coins as requested
    return m_coinsCollected >= 5 && m_coinsCollected > m_lastTriggerCoin;
}

void SurpriseBoxManager::onCoinCollected() {
    m_coinsCollected++;

    std::cout << "[SurpriseBox] Coins collected: " << m_coinsCollected
        << "/5 for next surprise" << std::endl;

    if (shouldTriggerSurprise()) {
        triggerSurprise();
    }
}

void SurpriseBoxManager::triggerSurprise() {
    if (!m_player || !m_entityManager || !m_world) {
        std::cerr << "[SurpriseBox] Cannot trigger - missing dependencies" << std::endl;
        return;
    }

    std::cout << "[SurpriseBox] Triggering surprise box!" << std::endl;

    // Show surprise box screen
    SurpriseGiftType selectedGift = m_surpriseScreen->showSurpriseBox();

    // Get player position for spawning
    auto* playerTransform = m_player->getComponent<Transform>();
    if (!playerTransform) {
        std::cerr << "[SurpriseBox] Player has no transform component" << std::endl;
        return;
    }

    // Calculate spawn position - above and slightly to the right of the player
    sf::Vector2f playerPos = playerTransform->getPosition();

    // Spawn gift above player so it falls down
    sf::Vector2f spawnPos = playerPos + sf::Vector2f(100.0f, -150.0f);

    // Spawn the selected gift
    spawnGiftEntity(selectedGift, spawnPos);

    // Update counter - reset after 5 coins
    m_lastTriggerCoin = m_coinsCollected;

    // Optional: Reset coin counter every 5 coins
    if (m_coinsCollected % 5 == 0) {
        std::cout << "[SurpriseBox] Ready for next surprise box at "
            << (m_coinsCollected + 5) << " coins" << std::endl;
    }
}

void SurpriseBoxManager::spawnGiftEntity(SurpriseGiftType giftType, const sf::Vector2f& position) {
    if (!m_entityManager || !m_world) {
        std::cerr << "[SurpriseBox] Cannot spawn gift - missing entity manager or world" << std::endl;
        return;
    }

    // Convert SurpriseGiftType to GiftEntity::GiftType
    GiftEntity::GiftType entityGiftType;
    std::string giftName;

    switch (giftType) {
    case SurpriseGiftType::LifeHeart:
        entityGiftType = GiftEntity::GiftType::LifeHeart;
        giftName = "Life Heart";
        break;
    case SurpriseGiftType::SpeedBoost:
        entityGiftType = GiftEntity::GiftType::SpeedBoost;
        giftName = "Speed Boost";
        break;
    case SurpriseGiftType::Shield:
        entityGiftType = GiftEntity::GiftType::Shield;
        giftName = "Shield";
        break;
    case SurpriseGiftType::RareCoin:
        entityGiftType = GiftEntity::GiftType::RareCoin;
        giftName = "Rare Coin";
        break;
    case SurpriseGiftType::ReverseMovement:
        entityGiftType = GiftEntity::GiftType::ReverseMovement;
        giftName = "Reverse Movement";
        break;
    case SurpriseGiftType::HeadwindStorm:
        entityGiftType = GiftEntity::GiftType::HeadwindStorm;
        giftName = "Headwind Storm";
        break;
    case SurpriseGiftType::Magnetic:
        entityGiftType = GiftEntity::GiftType::Magnetic;
        giftName = "Magnetic";
        break;
    default:
        entityGiftType = GiftEntity::GiftType::RareCoin;
        giftName = "Unknown Gift";
        break;
    }

    try {
        // Create gift entity with physics so it falls
        auto giftEntity = std::make_unique<GiftEntity>(
            g_nextEntityId++,
            entityGiftType,
            position.x,
            position.y,
            m_textures
        );

        // Add physics component to make it fall
        auto* physics = giftEntity->addComponent<PhysicsComponent>(*m_world, b2_dynamicBody);
        physics->createBoxShape(30.0f, 30.0f, 0.5f, 0.3f, 0.1f);
        physics->setPosition(position.x, position.y);

        // Make it fall slowly
        if (auto* body = physics->getBody()) {
            body->SetGravityScale(0.5f);
            body->SetLinearDamping(0.5f);
        }

        std::cout << "[SurpriseBox] Spawning " << giftName << " at position ("
            << position.x << ", " << position.y << ")" << std::endl;

        // Add to entity manager
        m_entityManager->addEntity(std::move(giftEntity));

    }
    catch (const std::exception& e) {
        std::cerr << "[SurpriseBox] Error spawning gift: " << e.what() << std::endl;
    }
}