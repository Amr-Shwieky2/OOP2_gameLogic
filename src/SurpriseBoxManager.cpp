#include "SurpriseBoxManager.h"
#include "LifeHeartGift.h"
#include "SpeedGift.h"
#include "ProtectiveShieldGift.h"
#include "RareCoinGift.h"
#include "ReverseMovementGift.h"
#include "HeadwindStormGift.h"
#include "Coin.h"
#include "Constants.h"
#include <iostream>
#include <cmath>
#include <App.h>
#include "SurpriseBoxScreen.h"
#include "MagneticGift.h"

SurpriseBoxManager::SurpriseBoxManager(TextureManagerType& textures, sf::RenderWindow& window)   
    : m_textures(textures)
    , m_window(&window)
    , m_gen(std::random_device{}())
{
    // إنشاء شاشة الصندوق
    m_surpriseScreen = std::make_unique<SurpriseBoxScreen>(window, textures);
}

void SurpriseBoxManager::setSpawnCallback(std::function<void(std::unique_ptr<GameObject>)> callback) {
    m_spawnCallback = callback;
}

bool SurpriseBoxManager::shouldTriggerSurprise() const {
    return m_coinsCollected >= (m_lastTriggerCoin + COINS_FOR_SURPRISE);
}

void SurpriseBoxManager::onCoinCollected() {
    m_coinsCollected++;

    if (shouldTriggerSurprise()) {
        triggerSurprise();
    }
}

void SurpriseBoxManager::triggerSurprise() {
    // عرض شاشة الصندوق المستقلة
    SurpriseGiftType selectedGift = m_surpriseScreen->showSurpriseBox();

    // إنشاء الهدية المختارة
    spawnGiftByType(selectedGift);

    // تحديث العداد
    m_lastTriggerCoin = m_coinsCollected;
    m_boxActive = false;
}

void SurpriseBoxManager::spawnGiftByType(SurpriseGiftType giftType) {

    sf::Vector2f playerCenter = m_player->getSpriteCenter();  //استخدم مركز اللاعب
    sf::Vector2f giftPos = playerCenter + sf::Vector2f(270.f, 0.f); 

    try {
        switch (giftType) {
        case SurpriseGiftType::LifeHeart:
            m_spawnCallback(std::make_unique<LifeHeartGift>(giftPos.x, giftPos.y, m_textures));
            break;
        case SurpriseGiftType::SpeedBoost:
            m_spawnCallback(std::make_unique<SpeedGift>(giftPos.x, giftPos.y, m_textures));
            break;
        case SurpriseGiftType::Shield:
            m_spawnCallback(std::make_unique<ProtectiveShieldGift>(giftPos.x, giftPos.y, m_textures));
            break;
        case SurpriseGiftType::RareCoin:
                m_spawnCallback(std::make_unique<RareCoinGift>(giftPos.x , giftPos.y, m_textures));
            break;
        case SurpriseGiftType::ReverseMovement:
            m_spawnCallback(std::make_unique<ReverseMovementGift>(giftPos.x, giftPos.y, m_textures));
            break;
        case SurpriseGiftType::HeadwindStorm:
            m_spawnCallback(std::make_unique<HeadwindStormGift>(giftPos.x, giftPos.y, m_textures));
            break;
        case SurpriseGiftType::Magnetic:
            m_spawnCallback(std::make_unique<MagneticGift>(giftPos.x, giftPos.y, m_textures));
            break;
        default:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y, m_textures));
            std::cout << " Default coin spawned!" << std::endl;
            break;
        }
    }
    catch (const std::exception& e) {
        std::cout << " Error spawning gift: " << e.what() << std::endl;
        m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y, m_textures));
    }
}

void SurpriseBoxManager::setPlayer(Player* player) {
    m_player = player;
}