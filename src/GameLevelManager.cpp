#include "GameLevelManager.h"
#include "EntityManager.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include <iostream>
#include <PlayerEntity.h>
#include "EntityFactory.h"

GameLevelManager::GameLevelManager() {
    // Add default levels
    m_levelManager.addLevel("level1.txt");
    m_levelManager.addLevel("level2.txt");

    std::cout << "[GameLevelManager] Created with " << m_levelManager.getLevelCount() << " levels" << std::endl;
}

void GameLevelManager::initialize(EntityManager& entityManager, PhysicsManager& physicsManager, TextureManager& textures) {
    m_entityManager = &entityManager;
    m_physicsManager = &physicsManager;
    m_textures = &textures;

    setupEventHandlers();
    std::cout << "[GameLevelManager] Initialized" << std::endl;
}

bool GameLevelManager::loadLevel(const std::string& levelPath) {
    if (!m_entityManager || !m_physicsManager || !m_textures) {
        std::cerr << "[GameLevelManager] Not initialized - cannot load level" << std::endl;
        return false;
    }

    std::cout << "[GameLevelManager] Loading level: " << levelPath << std::endl;

    // Clear existing entities
    m_entityManager->clear();

    // Reset transition state
    m_transitionPending = false;
    m_transitionTimer = 0.0f;
    m_needLevelSwitch = false;

    // FIX: Cast TextureManager& correctly
    TextureManager& textureManager = *m_textures;

    // Load level using the level loader
    bool success = m_levelLoader.loadFromFile(levelPath, *m_entityManager, m_physicsManager->getWorld(), textureManager);

    if (success) {
        // Check if player was created by level loading
        bool playerFound = false;
        for (auto* entity : m_entityManager->getAllEntities()) {
            if (dynamic_cast<PlayerEntity*>(entity)) {
                playerFound = true;
                break;
            }
        }

        // If no player in level, create default player (backwards compatibility)
        if (!playerFound) {
            std::cout << "[GameLevelManager] No player in level, creating default player" << std::endl;
            try {
                auto playerEntity = EntityFactory::instance().create("Player", 200.0f, 400.0f);
                if (playerEntity) {
                    m_entityManager->addEntity(std::move(playerEntity));
                    std::cout << "[GameLevelManager] Default player created successfully" << std::endl;
                }
                else {
                    std::cerr << "[GameLevelManager] Failed to create default player" << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[GameLevelManager] Error creating player: " << e.what() << std::endl;
            }
        }

        std::cout << "[GameLevelManager] Level loaded successfully: " << levelPath << std::endl;
    }
    else {
        std::cerr << "[GameLevelManager] Failed to load level: " << levelPath << std::endl;
    }

    return success;
}

bool GameLevelManager::loadNextLevel() {
    if (m_levelManager.hasNextLevel()) {
        std::string currentLevel = m_levelManager.getCurrentLevelPath();

        if (m_levelManager.loadNextLevel()) {
            std::string nextLevel = m_levelManager.getCurrentLevelPath();

            std::cout << "[GameLevelManager] Queuing next level: " << nextLevel << std::endl;

            // Queue the level switch instead of doing it immediately
            m_nextLevelPath = nextLevel;
            m_needLevelSwitch = true;

            EventSystem::getInstance().publish(
                LevelTransitionEvent(currentLevel, nextLevel, false)
            );

            return true;
        }
    }
    else {
        std::cout << "[GameLevelManager] No more levels - game complete!" << std::endl;
        EventSystem::getInstance().publish(
            LevelTransitionEvent(m_levelManager.getCurrentLevelPath(), "", true)
        );
    }

    return false;
}

bool GameLevelManager::reloadCurrentLevel() {
    std::string currentLevel = m_levelManager.getCurrentLevelPath();
    std::cout << "[GameLevelManager] Reloading current level: " << currentLevel << std::endl;
    return loadLevel(currentLevel);
}

const std::string& GameLevelManager::getCurrentLevelPath() const {
    return m_levelManager.getCurrentLevelPath();
}

std::size_t GameLevelManager::getCurrentLevelIndex() const {
    return m_levelManager.getCurrentIndex();
}

bool GameLevelManager::hasNextLevel() const {
    return m_levelManager.hasNextLevel();
}

std::size_t GameLevelManager::getLevelCount() const {
    return m_levelManager.getLevelCount();
}

void GameLevelManager::update(float deltaTime) {
    // Handle delayed level switching
    if (m_needLevelSwitch) {
        std::cout << "[GameLevelManager] Performing delayed level switch to: " << m_nextLevelPath << std::endl;
        m_needLevelSwitch = false;
        loadLevel(m_nextLevelPath);
        return;
    }

    // Handle level transition timing
    if (m_transitionPending) {
        m_transitionTimer += deltaTime;

        if (m_transitionTimer >= m_transitionDelay) {
            m_transitionPending = false;

            std::cout << "[GameLevelManager] Starting delayed level transition..." << std::endl;

            if (!loadNextLevel()) {
                std::cout << "[GameLevelManager] All levels completed!" << std::endl;
            }
        }
    }
}

void GameLevelManager::setupEventHandlers() {
    EventSystem::getInstance().subscribe<FlagReachedEvent>(
        [this](const FlagReachedEvent& event) {
            this->onFlagReached(event);
        }
    );

    EventSystem::getInstance().subscribe<LevelTransitionEvent>(
        [this](const LevelTransitionEvent& event) {
            this->onLevelTransition(event);
        }
    );
}

void GameLevelManager::onFlagReached(const FlagReachedEvent& event) {
    std::cout << "[GameLevelManager] Flag reached! Starting transition timer..." << std::endl;
    m_transitionPending = true;
    m_transitionTimer = 0.0f;
}

void GameLevelManager::onLevelTransition(const LevelTransitionEvent& event) {
    if (event.isGameComplete) {
        std::cout << "[GameLevelManager] Game completed!" << std::endl;
    }
    else {
        std::cout << "[GameLevelManager] Transitioning from " << event.fromLevel
            << " to " << event.toLevel << std::endl;
    }
}