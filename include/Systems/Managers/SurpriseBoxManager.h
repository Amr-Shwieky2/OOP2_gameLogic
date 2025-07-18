﻿#pragma once  
#include <SFML/Graphics.hpp>  
#include <functional>  
#include <memory>  
#include <random>  
#include "ResourceManager.h"  
#include "SurpriseBoxScreen.h"
#include "Entity.h"
#include <box2d/b2_body.h>
 
class EntityManager;
class PlayerEntity;

class SurpriseBoxManager {
public:

    static constexpr int COINS_FOR_SURPRISE = 1;

    SurpriseBoxManager(TextureManager& textures, sf::RenderWindow& window);
    ~SurpriseBoxManager() ;

    // Set the entity manager and physics world for spawning
    void setEntityManager(EntityManager* manager) { m_entityManager = manager; }
    void setPhysicsWorld(b2World* world) { m_world = world; }

    // Called when a coin is collected
    void onCoinCollected();

    // Set the player reference
    void setPlayer(PlayerEntity* player) { m_player = player; }

    // Reset coin counter (for new levels)
    void reset();

private:
    void triggerSurprise();
    bool shouldTriggerSurprise() const;
    void spawnGiftEntity(SurpriseGiftType giftType, const sf::Vector2f& position);

    TextureManager& m_textures;
    sf::RenderWindow* m_window;
    EntityManager* m_entityManager = nullptr;
    b2World* m_world = nullptr;
    PlayerEntity* m_player = nullptr;

    int m_coinsCollected = 0;
    int m_lastTriggerCoin = 0;

    std::unique_ptr<SurpriseBoxScreen> m_surpriseScreen;
    std::mt19937 m_gen;
};