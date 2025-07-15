#include "GameLevelManager.h"
#include "EntityManager.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "GameSession.h"
#include <iostream>
#include <PlayerEntity.h>
#include "EntityFactory.h"
#include "ResourcePaths.h"

//-------------------------------------------------------------------------------------
GameLevelManager::GameLevelManager() {
    m_levelManager.addLevel(ResourcePaths::LEVEL1);
}
//-------------------------------------------------------------------------------------
void GameLevelManager::initialize(EntityManager& entityManager, PhysicsManager& physicsManager, TextureManager& textures) {
    m_entityManager = &entityManager;
    m_physicsManager = &physicsManager;
    m_textures = &textures;

    setupEventHandlers();
}
//-------------------------------------------------------------------------------------
bool GameLevelManager::loadLevel(const std::string& levelPath) {
    if (!m_entityManager || !m_physicsManager || !m_textures) {
        return false;
    }

    try {
        m_transitionPending = false;
        m_transitionTimer = 0.0f;
        m_needLevelSwitch = false;

        if (g_currentSession) {
            g_currentSession->invalidateCachedPlayer();
        }
        m_entityManager->clear();
        TextureManager& textureManager = *m_textures;
        bool success = m_levelLoader.loadFromFile(levelPath, *m_entityManager, m_physicsManager->getWorld(), textureManager);

        if (success) {
            bool playerFound = false;
            for (auto* entity : m_entityManager->getAllEntities()) {
                if (auto* player = dynamic_cast<PlayerEntity*>(entity)) {
                    playerFound = true;
                    break;
                }
            }
            if (!playerFound) {
                try {
                    auto playerEntity = EntityFactory::instance().create("Player", 200.0f, 400.0f);
                    if (playerEntity) {
                        m_entityManager->addEntity(std::move(playerEntity));
                    }
                }
                catch (const std::exception& e) {
					std::cerr << "[ERROR] Failed to create default player entity: " << e.what() << std::endl;
                }
            }
        }

        return success;
    }
    catch (const std::exception& e) {
		std::cerr << "[ERROR] Failed to load level '" << levelPath << "': " << e.what() << std::endl;
        return false;
    }
}
//-------------------------------------------------------------------------------------
bool GameLevelManager::loadNextLevel() {
    if (m_levelManager.hasNextLevel()) {
        std::string currentLevel = m_levelManager.getCurrentLevelPath();

        if (m_levelManager.loadNextLevel()) {
            std::string nextLevel = m_levelManager.getCurrentLevelPath();

            m_nextLevelPath = nextLevel;
            m_needLevelSwitch = true;

            try {
                EventSystem::getInstance().publish(
                    LevelTransitionEvent(currentLevel, nextLevel, false)
                );
            }
            catch (const std::exception& e) {
				std::cerr << "[ERROR] Failed to publish level transition event: " << e.what() << std::endl;
            }

            return true;
        }
    }
    else {
        try {
            EventSystem::getInstance().publish(
                LevelTransitionEvent(m_levelManager.getCurrentLevelPath(), "", true)
            );
        }
        catch (const std::exception& e) {
			std::cerr << "[ERROR] Failed to publish game complete event: " << e.what() << std::endl;
            
        }
    }

    return false;
}
//-------------------------------------------------------------------------------------
bool GameLevelManager::reloadCurrentLevel() {
    std::string currentLevel = m_levelManager.getCurrentLevelPath();
    return loadLevel(currentLevel);
}
//-------------------------------------------------------------------------------------
const std::string& GameLevelManager::getCurrentLevelPath() const {
    return m_levelManager.getCurrentLevelPath();
}
//-------------------------------------------------------------------------------------
std::size_t GameLevelManager::getCurrentLevelIndex() const {
    return m_levelManager.getCurrentIndex();
}
//-------------------------------------------------------------------------------------
bool GameLevelManager::hasNextLevel() const {
    return m_levelManager.hasNextLevel();
}
//-------------------------------------------------------------------------------------
std::size_t GameLevelManager::getLevelCount() const {
    return m_levelManager.getLevelCount();
}
//-------------------------------------------------------------------------------------
void GameLevelManager::update(float deltaTime) {
    if (m_needLevelSwitch) {
        m_needLevelSwitch = false;
        try {
            loadLevel(m_nextLevelPath);
        }
        catch (const std::exception& e) {
			std::cerr << "[ERROR] Failed to switch to next level '" << m_nextLevelPath << "': " << e.what() << std::endl;
        }
        return;
    }

    // Handle level transition timing
    if (m_transitionPending) {
        m_transitionTimer += deltaTime;

        if (m_transitionTimer >= m_transitionDelay) {
            m_transitionPending = false;

            try {
                if (!loadNextLevel()) {
                }
            }
            catch (const std::exception& e) {
				std::cerr << "[ERROR] Failed to load next level: " << e.what() << std::endl;
            }
        }
    }
}
//-------------------------------------------------------------------------------------
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
		std::cerr << "[ERROR] Failed to set up event handlers: " << e.what() << std::endl;
    }
}
//-------------------------------------------------------------------------------------
void GameLevelManager::onFlagReached(const FlagReachedEvent&) {
    m_transitionPending = true;
    m_transitionTimer = 0.0f;
}
//-------------------------------------------------------------------------------------
void GameLevelManager::onLevelTransition(const LevelTransitionEvent& event) {
    if (event.isGameComplete) {
    }
    else {
    }
}
//-------------------------------------------------------------------------------------
void GameLevelManager::onWellEntered(const WellEnteredEvent& event) {
    try {
        std::string targetLevel = event.targetLevel;
        if (targetLevel.empty()) {
            targetLevel = ResourcePaths::DARK_LEVEL;
        }
        if (loadLevel(targetLevel)) {
        }
        else {
        }
    }
    catch (const std::exception& e) {
		std::cerr << "[ERROR] Failed to handle well entered event: " << e.what() << std::endl;
    }
}
//-------------------------------------------------------------------------------------