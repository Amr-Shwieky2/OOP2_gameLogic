// GameLevelManager.h
#pragma once
#include "LevelManager.h"
#include "LevelLoader.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include <memory>
#include "EntityManager.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"

/**
 * GameLevelManager - Single Responsibility: Handle all level operations
 *
 * Responsibilities:
 * - Load levels using LevelLoader
 * - Manage level progression using LevelManager
 * - Handle level transition events
 * - Coordinate level-related cleanup
 */
class GameLevelManager {
public:
    GameLevelManager();

    void initialize(EntityManager& entityManager, PhysicsManager& physicsManager, TextureManager& textures);

    // Level operations
    bool loadLevel(const std::string& levelPath);
    bool loadNextLevel();
    bool reloadCurrentLevel();

    // Level info
    const std::string& getCurrentLevelPath() const;
    std::size_t getCurrentLevelIndex() const;
    bool hasNextLevel() const;
    std::size_t getLevelCount() const;

    // Event handling
    void setupEventHandlers();
    void update(float deltaTime); // For transition timing

private:
    LevelManager m_levelManager;
    LevelLoader m_levelLoader;

    // Dependencies (injected)
    EntityManager* m_entityManager = nullptr;
    PhysicsManager* m_physicsManager = nullptr;
    TextureManager* m_textures = nullptr;

    // Event handlers
    void onFlagReached(const FlagReachedEvent& event);
    void onLevelTransition(const LevelTransitionEvent& event);

    void onWellEntered(const WellEnteredEvent& event);

    // Level transition state
    bool m_transitionPending = false;
    float m_transitionTimer = 0.0f;
    float m_transitionDelay = 2.0f;
    std::string m_nextLevelPath;
    bool m_needLevelSwitch = false;
};