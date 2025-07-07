#pragma once
#include <memory>
#include <Box2D/Box2D.h>
#include "EntityManager.h"
#include "MultiMethodCollisionSystem.h"
#include "RenderSystem.h"
#include "ResourceManager.h"
#include "LevelLoader.h"
#include <SurpriseBoxManager.h>
#include "LevelManager.h"
#include <GameEvents.h>

class PlayerEntity;

// Global pointer to the active game session for easy access
extern class GameSession* g_currentSession;

/**
 * GameSession - Manages the game entities and systems
 * Replaces GameWorld with component-based architecture
 */
class GameSession {
public:
    GameSession();
    ~GameSession();

    void initialize(TextureManager& textures, sf::RenderWindow& window);
    void loadLevel(const std::string& levelPath);

    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    // Entity access
    PlayerEntity* getPlayer() { return m_player; }
    EntityManager& getEntityManager() { return m_entityManager; }

    // Add entity to the world
    void spawnEntity(std::unique_ptr<Entity> entity);

    SurpriseBoxManager* getSurpriseBoxManager() { return m_surpriseBoxManager.get(); }
    bool loadNextLevel();
    void reloadCurrentLevel();

    LevelManager& getLevelManager() { return m_levelManager; }
    const std::string& getCurrentLevelName() const;

    void cleanupInactiveEntities();

private:
    void checkCollisions();
    bool areColliding(Entity& a, Entity& b);

    // Core systems
    b2World m_physicsWorld;
    EntityManager m_entityManager;
    MultiMethodCollisionSystem m_collisionSystem;
    RenderSystem m_renderSystem;
    LevelLoader m_levelLoader;
    // References
    PlayerEntity* m_player = nullptr;
    TextureManager* m_textures = nullptr;

    std::unique_ptr<SurpriseBoxManager> m_surpriseBoxManager;

    void setupEventHandlers();
    void onFlagReached(const FlagReachedEvent& event);
    void onLevelTransition(const LevelTransitionEvent& event);

    LevelManager m_levelManager; 

    // إضافة جديدة - متغيرات لإدارة الانتقال بين المستويات
    bool m_levelTransitionPending = false;
    float m_transitionDelay = 2.0f; // ثانيتان قبل الانتقال
    float m_transitionTimer = 0.0f;

    bool m_needLevelSwitch = false;
    std::string m_nextLevelPath;
};