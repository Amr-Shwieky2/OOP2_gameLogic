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

SurpriseBoxManager::SurpriseBoxManager(TextureManagerType& textures, sf::RenderWindow& window)   
    : m_textures(textures)
    , m_window(&window)
    , m_gen(std::random_device{}())
{    m_surpriseScreen = std::make_unique<SurpriseBoxScreen>(window, textures);
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
    // الحصول على موقع اللاعب بدلاً من موقع ثابت
    sf::Vector2f giftPos(400.0f, 400.0f); // موقع مؤقت - سنحسنه

    try {
        switch (giftType) {
        case SurpriseGiftType::LifeHeart:
            // بدلاً من إنشاء LifeHeartGift، أنشئ عملة حمراء مؤقتاً
            m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y, m_textures));
            break;

        case SurpriseGiftType::SpeedBoost:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x + 30.0f, giftPos.y, m_textures));
            break;

        case SurpriseGiftType::Shield:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x - 30.0f, giftPos.y, m_textures));
            break;

        case SurpriseGiftType::RareCoin:
            // 3 عملات ذهبية
            for (int i = 0; i < 3; ++i) {
                m_spawnCallback(std::make_unique<Coin>(giftPos.x + (i * 25.0f), giftPos.y - 20.0f, m_textures));
            }
            break;

        case SurpriseGiftType::ReverseMovement:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y + 30.0f, m_textures));
            break;

        case SurpriseGiftType::HeadwindStorm:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y - 30.0f, m_textures));
            break;

        default:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y, m_textures));
            break;
        }
    }
    catch (const std::exception& e) {
        std::cout << " Error spawning gift: " << e.what() << std::endl;
        m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y, m_textures));
    }
}