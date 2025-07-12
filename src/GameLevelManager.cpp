#include "GameLevelManager.h"
#include "GameSession.h"
#include "PlayerEntity.h"
#include "EntityFactory.h"
#include <iostream>

GameLevelManager::GameLevelManager() {
    // Add levels to sequence
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
    std::cout << "[GameLevelManager] Loading level: " << levelPath << std::endl;
    
    // Reset state
    m_transitionPending = false;
    m_transitionTimer = 0.0f;
    m_needLevelSwitch = false;
    
    // Clear entities
    m_entityManager->clear();
    
    // Load from file
    bool success = m_levelLoader.loadFromFile(levelPath, *m_entityManager, m_physicsManager->getWorld(), *m_textures);
    
    std::cout << "[GameLevelManager] Level " << (success ? "loaded successfully" : "failed to load") << std::endl;
    return success;
}

bool GameLevelManager::loadNextLevel() {
    if (m_levelManager.hasNextLevel()) {
        if (m_levelManager.loadNextLevel()) {
            std::string nextLevel = m_levelManager.getCurrentLevelPath();
            
            m_nextLevelPath = nextLevel;
            m_needLevelSwitch = true;
            
            return true;
        }
    }
    return false;
}

bool GameLevelManager::reloadCurrentLevel() {
    return loadLevel(m_levelManager.getCurrentLevelPath());
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
        m_needLevelSwitch = false;
        loadLevel(m_nextLevelPath);
    }
    
    // Handle transition timing
    if (m_transitionPending) {
        m_transitionTimer += deltaTime;
        
        if (m_transitionTimer >= m_transitionDelay) {
            m_transitionPending = false;
            loadNextLevel();
        }
    }
}

void GameLevelManager::setupEventHandlers() {
    try {
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
        
        EventSystem::getInstance().subscribe<WellEnteredEvent>(
            [this](const WellEnteredEvent& event) {
                this->onWellEntered(event);
            }
        );
    }
    catch (const std::exception& e) {
        std::cerr << "[GameLevelManager] Error setting up event handlers: " << e.what() << std::endl;
    }
}

void GameLevelManager::onFlagReached(const FlagReachedEvent&) {
    m_transitionPending = true;
    m_transitionTimer = 0.0f;
}

void GameLevelManager::onLevelTransition(const LevelTransitionEvent&) {
    // Nothing needed here
}

void GameLevelManager::onWellEntered(const WellEnteredEvent& event) {
    std::string targetLevel = event.targetLevel;
    
    if (targetLevel.empty() && m_levelManager.hasNextLevel()) {
        m_levelManager.loadNextLevel();
        targetLevel = m_levelManager.getCurrentLevelPath();
    }
    
    if (!targetLevel.empty()) {
        loadLevel(targetLevel);
    }
}