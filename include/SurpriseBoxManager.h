#pragma once  
#include <SFML/Graphics.hpp>  
#include <functional>  
#include <memory>  
#include <random>  
#include "ResourceManager.h"  
#include "Player.h"
#include "SurpriseBoxScreen.h"  // ← الآن آمن للاستيراد

// Forward declarations  
class GameObject;
using TextureManagerType = ResourceManager<sf::Texture>;

// حذف SurpriseGiftType من هنا - موجود في SurpriseBoxScreen.h

class SurpriseBoxManager {
public:
	static constexpr int COINS_FOR_SURPRISE = 5;

	SurpriseBoxManager(TextureManagerType& textures, sf::RenderWindow& window);
	~SurpriseBoxManager() = default;

	void setSpawnCallback(std::function<void(std::unique_ptr<GameObject>)> callback);
	void onCoinCollected();
	void setPlayer(Player* player);

private:
	void triggerSurprise();
	bool shouldTriggerSurprise() const;
	void spawnGiftByType(SurpriseGiftType giftType);

	TextureManagerType& m_textures;
	sf::RenderWindow* m_window;
	std::function<void(std::unique_ptr<GameObject>)> m_spawnCallback;

	int m_coinsCollected = 0;
	int m_lastTriggerCoin = 0;

	std::unique_ptr<SurpriseBoxScreen> m_surpriseScreen;
	std::mt19937 m_gen;
	bool m_boxActive = false;
	Player* m_player = nullptr;
};