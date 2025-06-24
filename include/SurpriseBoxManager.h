#pragma once  
#include <SFML/Graphics.hpp>  
#include <functional>  
#include <memory>  
#include <random>  
#include "ResourceManager.h"  
#include "Player.h"

// Forward declarations  
class GameObject;  
class SurpriseBoxScreen;  
using TextureManagerType = ResourceManager<sf::Texture>; 

enum class SurpriseGiftType {  
   LifeHeart,  
   SpeedBoost,  
   Shield,  
   RareCoin,  
   ReverseMovement,  
   HeadwindStorm,  
   Magnetic
};  

class SurpriseBoxManager {  
public:  
   static constexpr int COINS_FOR_SURPRISE = 5;  

   SurpriseBoxManager(TextureManagerType& textures, sf::RenderWindow& window); 
   ~SurpriseBoxManager() = default;

   // تسجيل callback لإنشاء الكائنات  
   void setSpawnCallback(std::function<void(std::unique_ptr<GameObject>)> callback);  

   // استدعاء عند جمع عملة  
   void onCoinCollected();  
   void setPlayer(Player* player);

private:  
   void triggerSurprise();  
   bool shouldTriggerSurprise() const;  
   void spawnGiftByType(SurpriseGiftType giftType);  

   // الأعضاء  
   TextureManagerType& m_textures;  // ← غيّر هنا
   sf::RenderWindow* m_window;
   std::function<void(std::unique_ptr<GameObject>)> m_spawnCallback;  

   // إحصائيات العملات  
   int m_coinsCollected = 0;  
   int m_lastTriggerCoin = 0;  

   // شاشة الصندوق المستقلة  
   std::unique_ptr<SurpriseBoxScreen> m_surpriseScreen;  

   // مولد عشوائي  
   std::mt19937 m_gen;  
   bool m_boxActive = false;  // للتتبع فقط  

   Player* m_player = nullptr;
};