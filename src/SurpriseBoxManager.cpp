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
{
    std::cout << "🎁 SurpriseBoxManager created! Trigger every " << COINS_FOR_SURPRISE << " coins." << std::endl;

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

    std::cout << "🪙 DEBUG - Score: " << (m_coinsCollected * 10)
        << ", Coins: " << m_coinsCollected
        << ", LastTrigger: " << m_lastTriggerCoin
        << ", NextSurprise: " << (m_lastTriggerCoin + COINS_FOR_SURPRISE)
        << ", BoxActive: " << (m_boxActive ? "YES" : "NO") << std::endl;

    if (shouldTriggerSurprise()) {
        std::cout << "🎊🎊🎊🎊🎊🎊 TRIGGERING SURPRISE BOX! 🎊🎊🎊🎊🎊🎊" << std::endl;
        std::cout << "Coins collected: " << m_coinsCollected
            << ", Required: " << (m_lastTriggerCoin + COINS_FOR_SURPRISE) << std::endl;

        triggerSurprise();
    }
}

void SurpriseBoxManager::triggerSurprise() {
    std::cout << "🎁 SURPRISE BOX TRIGGERED! 🎁" << std::endl;
    std::cout << "Opening surprise screen..." << std::endl;

    // عرض شاشة الصندوق المستقلة
    SurpriseGiftType selectedGift = m_surpriseScreen->showSurpriseBox();

    // إنشاء الهدية المختارة
    spawnGiftByType(selectedGift);

    // تحديث العداد
    m_lastTriggerCoin = m_coinsCollected;
    m_boxActive = false;

    std::cout << "🎉 Surprise complete! Next at " << (m_lastTriggerCoin + COINS_FOR_SURPRISE) << " coins." << std::endl;
}

void SurpriseBoxManager::spawnGiftByType(SurpriseGiftType giftType) {
    std::cout << "🎁 Spawning gift type: " << static_cast<int>(giftType) << std::endl;

    // الحصول على موقع اللاعب بدلاً من موقع ثابت
    sf::Vector2f giftPos(400.0f, 400.0f); // موقع مؤقت - سنحسنه

    try {
        switch (giftType) {
        case SurpriseGiftType::LifeHeart:
            // بدلاً من إنشاء LifeHeartGift، أنشئ عملة حمراء مؤقتاً
            m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y, m_textures));
            std::cout << "❤️ Life Heart (as coin) spawned!" << std::endl;
            break;

        case SurpriseGiftType::SpeedBoost:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x + 30.0f, giftPos.y, m_textures));
            std::cout << "🚀 Speed Boost (as coin) spawned!" << std::endl;
            break;

        case SurpriseGiftType::Shield:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x - 30.0f, giftPos.y, m_textures));
            std::cout << "🛡️ Shield (as coin) spawned!" << std::endl;
            break;

        case SurpriseGiftType::RareCoin:
            // 3 عملات ذهبية
            for (int i = 0; i < 3; ++i) {
                m_spawnCallback(std::make_unique<Coin>(giftPos.x + (i * 25.0f), giftPos.y - 20.0f, m_textures));
            }
            std::cout << "⭐ Rare Coin (as 3 coins) spawned!" << std::endl;
            break;

        case SurpriseGiftType::ReverseMovement:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y + 30.0f, m_textures));
            std::cout << "🔄 Reverse Movement (as coin) spawned!" << std::endl;
            break;

        case SurpriseGiftType::HeadwindStorm:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y - 30.0f, m_textures));
            std::cout << "🌪️ Headwind Storm (as coin) spawned!" << std::endl;
            break;

        case SurpriseGiftType::MultipleCoins:
            // إنشاء 5 عملات في دائرة
            for (int i = 0; i < 5; ++i) {
                float angle = (i * 2.0f * 3.14159f) / 5.0f;
                float radius = 40.0f;
                float x = giftPos.x + cos(angle) * radius;
                float y = giftPos.y + sin(angle) * radius;
                m_spawnCallback(std::make_unique<Coin>(x, y, m_textures));
            }
            std::cout << "💰💰💰 Multiple Coins spawned!" << std::endl;
            break;

        default:
            m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y, m_textures));
            std::cout << "💰 Default coin spawned!" << std::endl;
            break;
        }
    }
    catch (const std::exception& e) {
        std::cout << "❌ Error spawning gift: " << e.what() << std::endl;
        m_spawnCallback(std::make_unique<Coin>(giftPos.x, giftPos.y, m_textures));
        std::cout << "💰 Emergency coin spawned!" << std::endl;
    }
}