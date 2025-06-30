// SurpriseBoxManager.cpp
#include "SurpriseBoxManager.h"
#include "EntityManager.h"
#include "EntityFactory.h"
#include "PlayerEntity.h"
#include "GiftEntity.h"
#include "Transform.h"
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

    // Subscribe to item collection events and count coins
    EventSystem::getInstance().subscribe<ItemCollectedEvent>(
        [this](const ItemCollectedEvent& event) {
            if (event.type == ItemCollectedEvent::ItemType::Coin) {
                this->onCoinCollected();
            }
        }
    );
}

bool SurpriseBoxManager::shouldTriggerSurprise() const {
    return m_coinsCollected >= (m_lastTriggerCoin + COINS_FOR_SURPRISE);
}

void SurpriseBoxManager::onCoinCollected() {
    m_coinsCollected++;

    std::cout << "[SurpriseBox] Coins collected: " << m_coinsCollected
        << "/" << (m_lastTriggerCoin + COINS_FOR_SURPRISE) << std::endl;

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

    // Calculate spawn position - slightly to the right of the player and on
    // the same vertical level so the gift appears on the ground
    sf::Vector2f playerPos = playerTransform->getPosition();
    sf::Vector2f spawnPos = playerPos + sf::Vector2f(50.0f, 0.0f);

    // Spawn the selected gift
    spawnGiftEntity(selectedGift, spawnPos);

    // Update counter
    m_lastTriggerCoin = m_coinsCollected;
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
        // Create gift entity
        auto giftEntity = std::make_unique<GiftEntity>(
            g_nextEntityId++,
            entityGiftType,
            position.x,
            position.y,
            m_textures
        );

        // Add physics if needed (optional - gifts might just be static)
        // You could add a PhysicsComponent here if you want gifts to fall

        std::cout << "[SurpriseBox] Spawning " << giftName << " at position ("
            << position.x << ", " << position.y << ")" << std::endl;

        // Add to entity manager
        m_entityManager->addEntity(std::move(giftEntity));

        // Publish event for UI notification
        EventSystem::getInstance().publish(
            ItemCollectedEvent(
                m_player->getId(),
                g_nextEntityId - 1,  // The gift's ID
                ItemCollectedEvent::ItemType::Gift
            )
        );

    }
    catch (const std::exception& e) {
        std::cerr << "[SurpriseBox] Error spawning gift: " << e.what() << std::endl;
    }
}